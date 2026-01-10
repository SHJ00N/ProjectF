#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

struct DirLight
{
    vec4 colorIntensity;
    vec3 direction;
};

layout (std140, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
};

layout (std140, binding = 1) uniform DirLightBlock
{
    DirLight dirLight;
    int hasDirLight;
};

// gbuffer textures
uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoAO;
uniform sampler2D gRoughMetal;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

const float PI = 3.14159265359;

// get fragmenet world position
vec3 ReconstructWorldPos(vec2 uv, float depth)
{
    vec4 ndc;
    ndc.xy = uv * 2.0 - 1.0;
    ndc.z  = depth * 2.0 - 1.0;
    ndc.w  = 1.0;

    mat4 invProjection = inverse(projection);
    mat4 invView = inverse(view);

    vec4 viewPos = invProjection * ndc;
    viewPos /= viewPos.w;

    vec4 worldPos = invView * viewPos;
    return worldPos.xyz;
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// calculate light
// reflectance equation -------------------------------------------------------
vec3 reflectanceEquation(vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic, vec3 L, vec3 radiance)
{
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;

    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;

    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0); 

    vec3 result = (kD * albedo / PI + specular) * radiance * NdotL;
    return result;
}
// direction ------------------------------------------------------------------
vec3 calculateDirectionLight(vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic)
{
    // calculate per-light radiance
    vec3 L = normalize(-dirLight.direction);
    vec3 radiance = dirLight.colorIntensity.xyz * dirLight.colorIntensity.w;
    
    return reflectanceEquation(V, N, F0, albedo, roughness, metallic, L, radiance);
}
// point ----------------------------------------------------------------------
vec3 calculatePointLight(vec3 lightPosition, vec3 lightColor, vec3 WorldPos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic)
{
    vec3 L = normalize(lightPosition - WorldPos);
    float distance = length(lightPosition - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    return reflectanceEquation(V, N, F0, albedo, roughness, metallic, L, radiance);
}

void main()
{
    float Depth = texture(gDepth, TexCoords).r;
    if(Depth == 1.0) discard;

    vec3 WorldPos = ReconstructWorldPos(TexCoords, Depth);
    vec3 N = texture(gNormal, TexCoords).rgb;
    N = normalize(N * 2.0 - 1.0);
    vec3 V = normalize(cameraPos - WorldPos);
    vec3 R = reflect(-V, N); 

    vec3 albedo = texture(gAlbedoAO, TexCoords).rgb;
    float ao = texture(gAlbedoAO, TexCoords).a;
    float roughness = clamp(texture(gRoughMetal, TexCoords).r, 0.04, 1.0);
    float metallic = texture(gRoughMetal, TexCoords).g;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    if(hasDirLight == 1) Lo += calculateDirectionLight(V, N, F0, albedo, roughness, metallic);
    for(int i = 0; i < 4; ++i) 
    {
        Lo += calculatePointLight(lightPositions[i], lightColors[i], WorldPos, V, N, F0, albedo, roughness, metallic);
    }   

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    float maxMip = float(textureQueryLevels(prefilterMap) - 1);
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * maxMip).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));

    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}