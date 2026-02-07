#version 460 core
out float FragColor;

in vec2 TexCoords;

layout (std140, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    float cameraFov;
};

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.025;

const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0);

// get view position from depth
vec3 ReconstructViewPos(vec2 uv, float depth)
{
    vec4 ndc;
    ndc.xy = uv * 2.0 - 1.0;
    ndc.z  = depth * 2.0 - 1.0;
    ndc.w  = 1.0;

    mat4 invProjection = inverse(projection);

    vec4 viewPos = invProjection * ndc;
    viewPos.xyz /= viewPos.w;

    return viewPos.xyz;
}

void main()
{
    float depth = texture(gDepth, TexCoords).r;
    vec3 fragPos = ReconstructViewPos(TexCoords, depth);

    vec3 worldNormal = texture(gNormal, TexCoords).rgb;
    worldNormal = normalize(worldNormal * 2.0 - 1.0);
    vec3 normal = normalize(mat3(view) * worldNormal);

    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth = texture(gDepth, offset.xy).r;
        vec3 sampleViewPos = ReconstructViewPos(offset.xy, sampleDepth);
        float sampleZ = sampleViewPos.z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleZ));
        occlusion += (sampleZ >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}