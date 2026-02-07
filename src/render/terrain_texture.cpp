#include "render/terrain_texture.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

TerrainTexture::TerrainTexture()
{
}

TerrainTexture::TerrainTexture(const std::string &albedoPath, const std::string &normalPath, const std::string &roughenssPath, bool gamma)
{
    // load albedo
    if(gamma) AlbedoMap = loadSRGBTextureFromFile(albedoPath);
    else AlbedoMap = loadTextureFromFile(albedoPath);
    // load normal
    NormalMap = loadTextureFromFile(normalPath);
    // load roughness
    RoughnessMap = loadTextureFromFile(roughenssPath);
}

void TerrainTexture::Clear()
{
    glDeleteTextures(1, &AlbedoMap);
    glDeleteTextures(1, &NormalMap);
    glDeleteTextures(1, &RoughnessMap);
}

unsigned int TerrainTexture::loadTextureFromFile(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if(data){
        GLenum internalFormat;
        GLenum dataFormat;
        if(nrComponents == 1){
            internalFormat = dataFormat = GL_RED;
        }
        else if(nrComponents == 3){
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
        
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

unsigned int TerrainTexture::loadSRGBTextureFromFile(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if(data){
        GLenum internalFormat;
        GLenum dataFormat;
        if(nrComponents == 1){
            internalFormat = dataFormat = GL_RED;
        }
        else if(nrComponents == 3){
            internalFormat = GL_SRGB;
            dataFormat = GL_RGB;
        }
        else if(nrComponents == 4){
            internalFormat = GL_SRGB_ALPHA;
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