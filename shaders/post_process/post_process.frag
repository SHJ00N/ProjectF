#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D dirtMaskTexture;
uniform float exposure;
uniform float bloomStrength = 0.2f;
uniform float dirtMaskIntensity = 20.0f;
uniform int state = 0;

void main()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    vec3 result = hdrColor + bloomColor * bloomStrength; // linear interpolation

    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);

    // lens dirt
    vec3 dirt = texture(dirtMaskTexture, TexCoords).rgb;
    result += bloomColor * dirt * dirtMaskIntensity;

    // also gamma correct while we're at it
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    if(state == 1)
    {
        float average = 0.2126 * result.r + 0.7152 * result.g + 0.0722 * result.b;
        result = vec3(average, average, average);
    }

    FragColor = vec4(result, 1.0);
}