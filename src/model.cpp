#include "model.h"

void Model::Draw(Shader &shader){
    for(unsigned int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);
}

void Model::Clear()
{
    for(auto& mesh : meshes) mesh.Clear();
    for(auto& texture : texturesLoaded) glDeleteTextures(1, &texture.id);

    meshes.clear();
    texturesLoaded.clear();
}

void Model::LoadModel(string const &path, bool gamma){
    this->gammaCorrection = gamma;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << "Error::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    int count = 0;
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene){
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

std::map<std::string, BoneInfo>& Model::GetBoneInfoMap(){
    return m_BoneInfoMap;
}

int& Model::GetBoneCount(){
    return m_BoneCounter;
}

void Model::SetVertexBoneDataToDefault(Vertex &vertex){
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;
        
        SetVertexBoneDataToDefault(vertex);

        vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
        if(mesh->HasNormals()) vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

        if(mesh->HasTextureCoords(0)){
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.TexCoords = glm::vec2(0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent   = AssimpGLMHelpers::GetGLMVec(mesh->mTangents[i]);
            vertex.Bitangent = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i]);
        }
        
        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    // legacy model
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    // modern model
    if(textures.size() == 0)
    {
        string name = string(material->GetName().C_Str());
        Texture diffuseMap = loadModernMaterialTextures((name + "_Albedo.tga").c_str(), "texture_diffuse");
        if(diffuseMap.id) textures.push_back(diffuseMap);
        Texture normalMap = loadModernMaterialTextures((name + "_Normal.tga").c_str(), "texture_normal");
        if(normalMap.id) textures.push_back(normalMap);
        Texture roughMap = loadModernMaterialTextures((name + "_Roughness.tga").c_str(), "texture_roughness");
        if(roughMap.id) textures.push_back(roughMap);
        Texture metalMap = loadModernMaterialTextures((name + "_Metallic.tga").c_str(), "texture_metallic");
        if(metalMap.id) textures.push_back(metalMap);
        Texture aoMap = loadModernMaterialTextures((name + "_Ambient.tga").c_str(), "texture_ambient");
        if(aoMap.id) textures.push_back(aoMap);
    }
    
    if(mesh->HasBones()) ExtractBoneWeightForVertices(vertices, mesh, scene);

    return Mesh(vertices, indices, textures);
}

void Model::SetVertexBoneData(Vertex &vertex, int boneID, float weight){
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
        if(vertex.m_BoneIDs[i] < 0){
            vertex.m_BoneIDs[i] = boneID;
            vertex.m_Weights[i] = weight;
            break;
        }
    }
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene){
    auto& boneInfoMap = m_BoneInfoMap;
    int &boneCount = m_BoneCounter;

    for(int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex){
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if(boneInfoMap.find(boneName) == boneInfoMap.end()){
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        } else {
            boneID = boneInfoMap[boneName].id;
        }

        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for(int weightIndex = 0; weightIndex < numWeights; ++weightIndex){
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

// legacy model's texture load function
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName){
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for(unsigned int j = 0; j < texturesLoaded.size(); j++){
            if(strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0){
                textures.push_back(texturesLoaded[j]);
                skip = true;
                break;
            }
        }

        if(!skip){
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory, aiTextureType_DIFFUSE == type ? this->gammaCorrection : false);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
    
    return textures;
}

// modern model's texture load function
Texture Model::loadModernMaterialTextures(const char *path, string typeName)
{
    bool skip = false;
    for(unsigned int i = 0; i < texturesLoaded.size(); i++)
    {
        if(strcmp(texturesLoaded[i].path.data(), path) == 0)
        {
            return texturesLoaded[i];
        }
    }

    Texture texture;
    if(!skip)
    {
        texture.id = TextureFromFile(path, this->directory, typeName == "texture_diffuse" ? this->gammaCorrection : false);
        texture.type = typeName;
        texture.path = path;
        texturesLoaded.push_back(texture);
    }
    return texture;
}

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false){
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        //cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}