#version 460 core
layout(vertices=4) out;

in vec2 TexCoord[];
out vec2 TextureCoord[];

layout (std140, binding = 2) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

layout (std140, binding = 3) uniform CSMBlock
{
    float cascadePlaneDistances[16];
    int cascadeCount;   // number of frusta - 1
};

uniform mat4 model;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    if(gl_InvocationID == 0)
    {
        const int MIN_TESS_LEVEL = 12;
        const int MAX_TESS_LEVEL = 24;

        vec3 worldPos0 = vec3(model * gl_in[0].gl_Position);
        vec3 worldPos1 = vec3(model * gl_in[1].gl_Position);
        vec3 worldPos2 = vec3(model * gl_in[2].gl_Position);
        vec3 worldPos3 = vec3(model * gl_in[3].gl_Position);

        vec3 center = (worldPos0 + worldPos1 + worldPos2 + worldPos3) * 0.25;

        float minDepth = 1e10;

        for (int c = 0; c < cascadeCount; ++c)
        {
            vec4 lp = lightSpaceMatrices[c] * vec4(center, 1.0);
            float depth = abs(lp.z / lp.w);
            minDepth = min(minDepth, depth);
        }

        float t = clamp(minDepth / 100.0, 0.0, 1.0);
        float tess = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, t);

        gl_TessLevelOuter[0] = tess;
        gl_TessLevelOuter[1] = tess;
        gl_TessLevelOuter[2] = tess;
        gl_TessLevelOuter[3] = tess;

        gl_TessLevelInner[0] = tess;
        gl_TessLevelInner[1] = tess;
    }
}