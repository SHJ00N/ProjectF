#include "terrain.h"

Terrain::Terrain() : HeightScale(64.0f), WorldScale(1.0f), Rez(20)
{
}

Terrain::Terrain(const char *diffuseMapFile, const char *heightMapFile, const char *normalMapFile, const char *roughMapFile, float heightScale, float worldScale, unsigned int rez) : HeightScale(heightScale), WorldScale(worldScale), Rez(rez)
{
    // load texture
    DiffuseMap = textureFromFile(diffuseMapFile);
    RoughnessMap = textureFromFile(roughMapFile);
    HeightMap = loadHeightMap(heightMapFile);
    NormalMap = loadNormalMap(normalMapFile);

    // generate vertex
    for(unsigned int i = 0; i < rez; i++)
    {
        for(unsigned int j = 0; j < rez; j++)
        {
            Vertices.push_back(-heightMapWidth * WorldScale / 2.0f + heightMapWidth * i * WorldScale / (float)rez);
            Vertices.push_back(0.0f);
            Vertices.push_back(-heightMapHeight * WorldScale / 2.0f + heightMapHeight * j * WorldScale / (float)rez);
            Vertices.push_back(i / (float)rez);
            Vertices.push_back(j / (float)rez);

            Vertices.push_back(-heightMapWidth * WorldScale / 2.0f + heightMapWidth * (i + 1) * WorldScale / (float)rez);
            Vertices.push_back(0.0f);
            Vertices.push_back(-heightMapHeight * WorldScale / 2.0f + heightMapHeight * j * WorldScale / (float)rez);
            Vertices.push_back((i + 1) / (float)rez);
            Vertices.push_back(j / (float)rez);

            Vertices.push_back(-heightMapWidth * WorldScale / 2.0f + heightMapWidth * i * WorldScale / (float)rez);
            Vertices.push_back(0.0f);
            Vertices.push_back(-heightMapHeight * WorldScale / 2.0f + heightMapHeight * (j + 1) * WorldScale / (float)rez);
            Vertices.push_back(i / (float)rez);
            Vertices.push_back((j + 1) / (float)rez);

            Vertices.push_back(-heightMapWidth * WorldScale / 2.0f + heightMapWidth * (i + 1) * WorldScale / (float)rez);
            Vertices.push_back(0.0f);
            Vertices.push_back(-heightMapHeight * WorldScale / 2.0f + heightMapHeight * (j + 1) * WorldScale / (float)rez);
            Vertices.push_back((i + 1) / (float)rez);
            Vertices.push_back((j + 1) / (float)rez);
        }
    }

    // set VAO
    setUpTerrain();
}

void Terrain::Clear()
{
    // delete textures
    glDeleteTextures(1, &DiffuseMap);
    glDeleteTextures(1, &HeightMap);
    glDeleteTextures(1, &NormalMap);
    glDeleteTextures(1, &RoughnessMap);

    // delete buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

float Terrain::GetLocalHeight(float worldX, float worldZ)
{
    glm::vec2 localPos = transformLocalCoord(worldX, worldZ);

    return bilinearInterpolate(localPos.x, localPos.y);
}

glm::vec3 Terrain::GetNormal(float worldX, float worldZ)
{
    glm::vec2 localPos = transformLocalCoord(worldX, worldZ);

    float hL = bilinearInterpolate(localPos.x - 1.0f, localPos.y);
    float hR = bilinearInterpolate(localPos.x + 1.0f, localPos.y);
    float hD = bilinearInterpolate(localPos.x, localPos.y - 1.0f);
    float hU = bilinearInterpolate(localPos.x, localPos.y + 1.0f);

    glm::vec3 normal;
    normal.x = hL - hR;
    normal.y = 2.0f;
    normal.z = hD - hU;

    return glm::normalize(normal);
}

void Terrain::Render(Shader &shader)
{
    shader.SetInteger("heightMap", 0);
    shader.SetInteger("texture_diffuse", 1);
    shader.SetInteger("texture_normal", 2);
    shader.SetInteger("texture_roughness", 3);
    shader.SetFloat("heightScale", HeightScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, HeightMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, NormalMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, RoughnessMap);

    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * Rez * Rez);

    glBindVertexArray(0);
}

void Terrain::RenderShadow(Shader &shadowShader)
{
    shadowShader.SetInteger("heightMap", 0);
    shadowShader.SetFloat("heightScale", HeightScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, HeightMap);

    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * Rez * Rez);

    glBindVertexArray(0);
}

void Terrain::setUpTerrain()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), &Vertices[0], GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

    glBindVertexArray(0);
}

unsigned int Terrain::textureFromFile(const char *path, bool gamma){
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if(data){
        GLenum internalFormat;
        GLenum dataFormat;
        if(nrComponents == 1){
            internalFormat = dataFormat = GL_RED;
        }
        else if(nrComponents == 3){
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if(nrComponents == 4){
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

unsigned int Terrain::loadHeightMap(const char *heightMapFile)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int nrComponents;
    unsigned short *data = stbi_load_16(heightMapFile, &heightMapWidth, &heightMapHeight, &nrComponents, 1);
    if(data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, heightMapWidth, heightMapHeight, 0, GL_RED, GL_UNSIGNED_SHORT, data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        heightData.assign(data, data + heightMapWidth * heightMapHeight);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load height map at path: " << heightMapFile << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

unsigned int Terrain::loadNormalMap(const char *normalMapFile)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(normalMapFile, &width, &height, &nrComponents, 0);
    if(data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if(nrComponents == 3){
            internalFormat = GL_RGB;
            dataFormat = GL_RGB;
        }
        else if(nrComponents == 4){
            internalFormat = GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        normalData.assign(data, data + width * height * nrComponents);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load normal map at path: " << normalMapFile << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

float Terrain::bilinearInterpolate(float x, float z)
{
    int x0 = (int)floor(x);
    int z0 = (int)floor(z);
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    float h00 = getHeightFromHeightMap(x0, z0);
    float h10 = getHeightFromHeightMap(x1, z0);
    float h01 = getHeightFromHeightMap(x0, z1);
    float h11 = getHeightFromHeightMap(x1, z1);

    float fx = x - x0;
    float fz = z - z0;

    float h0 = glm::mix(h00, h10, fx);
    float h1 = glm::mix(h01, h11, fx);

    return glm::mix(h0, h1, fz);
}

float Terrain::getHeightFromHeightMap(int x, int z)
{
    x = glm::clamp(x, 0, heightMapWidth  - 1);
    z = glm::clamp(z, 0, heightMapHeight - 1);

    int index = z * heightMapWidth + x;
    float height = heightData[index] / 65535.0f;

    return height * HeightScale;
}

glm::vec2 Terrain::transformLocalCoord(float x, float z)
{
    x /= WorldScale;
    z /= WorldScale;
    
    float halfW = (heightMapWidth  - 1) * 0.5f;
    float halfH = (heightMapHeight - 1) * 0.5f;

    float tx = x + halfW;
    float tz = z + halfH;

    return {tx, tz};
}