#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in mat3 TBN;
in vec3 FragPos;

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
};

struct PointLight{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform PointLight pointLight;
uniform Material material;

void main()
{   
    vec3 tangentLightPos = TBN * pointLight.position;
    vec3 tangentViewPos = TBN * viewPos;
    vec3 tangentFragPos = TBN * FragPos;
    vec3 tangentNormal = texture(material.texture_normal1, TexCoords).rgb;
    tangentNormal = normalize(tangentNormal * 2.0f - 1.0f); 

    vec3 ambient = pointLight.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    
    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(tangentNormal, lightDir), 0.0f);
    vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    vec3 reflectDir = reflect(-lightDir, tangentNormal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(tangentNormal, halfwayDir), 0.0), 32.0);
    vec3 specular = pointLight.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    float distance = length(tangentLightPos - tangentFragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor = texture(material.texture_diffuse1, TexCoords);
}