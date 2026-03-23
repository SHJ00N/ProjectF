#version 460 core
out vec4 FragColor;

in vec2 UV;
in float lifeTime;

uniform sampler2D particleImage;
uniform sampler2D noiseTexture;
uniform float screenWidth;
uniform float screenHeight;

uniform vec3 color;

void main()
{
    vec2 uv = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);

    float life = clamp(lifeTime, 0.0, 1.0);

    float n1 = texture(noiseTexture, UV * 0.8).r;
    float n2 = texture(noiseTexture, UV * 2.5).r;

    float noise = n1 * 0.75 + n2 * 0.25;
    float breakup = smoothstep(0.2, 0.7, noise);
    float alpha = pow(life, 2.5);

    vec4 texColor = texture(particleImage, UV); 
    vec3 freshBlood = vec3(0.4, 0.05, 0.05);
    vec3 dryBlood   = vec3(0.15, 0.01, 0.005);
    float color =   pow(1.0 - life, 0.6);
    vec3 bloodColor = mix(freshBlood, dryBlood, color);
    bloodColor = pow(bloodColor, vec3(1.6));
    FragColor = vec4(texColor.rgb * 0.6 * bloodColor, texColor.a * alpha * breakup);
}