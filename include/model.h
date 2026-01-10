#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"
#include "animdata.h"
#include "assimp_glm_helpers.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);

class Model{
public:
    vector<Texture> texturesLoaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(){ };
    void Draw(Shader &shader);
    void LoadModel(string const &path, bool gamma = false);
    void Clear();

    std::map<std::string, BoneInfo>& GetBoneInfoMap();
    int& GetBoneCount();

private:
    std::map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene);

    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    Texture loadModernMaterialTextures(const char *path, string typeName);
};

#endif