#version 460 core
layout (location = 0) out vec3 gNormal; 
layout (location = 1) out vec4 gAlbedoAO;
layout (location = 2) out vec3 gRM;
layout (location = 3) out vec2 gTypeSlope;

in vec2 TexCoords;
in mat3 TBN;

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_ambient1;
    sampler2D texture_roughness1;
    sampler2D texture_metallic1;
};

uniform Material material;
uniform int hasAO;
uniform int hasMetallic;
uniform int hasRoughness;

void main()
{
    vec3 Normal = texture(material.texture_normal1, TexCoords).rgb;
    Normal = normalize(Normal * 2.0f - 1.0f);
    Normal = normalize(TBN * Normal);
    gNormal = Normal * 0.5f + 0.5f;

    gAlbedoAO.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
    gAlbedoAO.a   = hasAO == 1 ? texture(material.texture_ambient1, TexCoords).r : 1.0;

    gRM.r = hasRoughness == 1 ? texture(material.texture_roughness1, TexCoords).r : 0.5;
    gRM.g = hasMetallic == 1 ? texture(material.texture_metallic1, TexCoords).r : 0.0;

    gTypeSlope.r = 0.0;
}