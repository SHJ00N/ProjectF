#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in mat3 TBN;
in vec3 ViewPos;
in vec3 FragPos;

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight
{
    Light light;
    vec3 direction;
};

layout (std140, binding = 1) uniform DirLightBlock
{
    DirLight dirLight;
    int hasDirLight;
};

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

Light CalcLight(Light light, vec3 lightDir, vec3 viewPos, vec3 fragPos, vec3 normal);
vec3 CalcDirLight(DirLight light, vec3 viewPos, vec3 fragPos, vec3 normal);

void main()
{
    vec3 Normal = texture(normalMap, TexCoords).rgb;
    Normal = normalize(Normal * 2.0f - 1.0f);
    Normal = normalize(TBN * Normal);

    vec3 result = vec3(0.0f);
    if(hasDirLight == 1) result += CalcDirLight(dirLight, ViewPos, FragPos, Normal);

    FragColor = vec4(result, 1.0f);
}

Light CalcLight(Light light, vec3 lightDir, vec3 viewPos, vec3 fragPos, vec3 normal)
{
    Light temp;
    temp.ambient = light.ambient * texture(diffuseMap, TexCoords).rgb;

    float diff = max(dot(lightDir, normal), 0.0f);
    temp.diffuse = light.diffuse * diff * texture(diffuseMap, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
    temp.specular = light.specular * spec * texture(specularMap, TexCoords).rgb;

    return temp;
}

vec3 CalcDirLight(DirLight light, vec3 viewPos, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);
    Light result = CalcLight(light.light, lightDir, viewPos, fragPos, normal);

    return result.ambient + result.diffuse + result.specular;
}