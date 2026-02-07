#version 460 core
layout (location = 0) out vec3 gNormal; 
layout (location = 1) out vec4 gAlbedoAO;
layout (location = 2) out vec3 gRM;
layout (location = 3) out vec2 gTypeSlope;

in vec2 TexCoords;
in mat3 TBN;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughness;
uniform sampler2D texture_slopeLighter;

void main()
{
    //vec3 Normal = texture(texture_normal, TexCoords).rgb;
    //Normal = normalize(Normal * 2.0f - 1.0f);
    //Normal = normalize(TBN * Normal);
    gNormal = Normal * 0.5f + 0.5f;

    gAlbedoAO.rgb = texture(texture_diffuse, TexCoords).rgb;
    gAlbedoAO.a   = 1.0;

    gRM.r = 1.0 - clamp(0.0, 0.1, texture(texture_roughness, TexCoords).r);
    gRM.g = 0.0;

    gTypeSlope.r = 1.0;
    gTypeSlope.g = texture(texture_slopeLighter, TexCoords).r;
}