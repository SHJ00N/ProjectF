#version 460 core
in float Height;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseMap;

void main()
{
    float h = (Height + 16) / 64.0f;
    FragColor = vec4(h, h, h, 1.0);
    FragColor = texture(diffuseMap, TexCoord);
}