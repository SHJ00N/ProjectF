#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>

#include "resource_manager.h"
#include "animator.h"
#include "game_object.h"
#include "skeletal_mesh_renderer.h"
#include "static_mesh_renderer.h"
#include "terrain.h"
#include "chunk_grid.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 3000.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float smoothSpeed = 10.0f; // 값 클수록 즉각 반응

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// model renderer
SkeletalMeshRenderer Renderer;
StaticMeshRenderer staticMeshRenderer;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    ResourceManager::LoadShader("shaders/sample.vert", "shaders/sample.frag", nullptr, nullptr, nullptr, "modelLoading");
    ResourceManager::LoadShader("shaders/gpuheight.vert", "shaders/gpuheight.frag", nullptr, "shaders/gpuheight.tcs", "shaders/gpuheight.tes", "terrainShader");
    
    // load models
    // -----------
    ResourceManager::LoadModel("resources/object/Vampire A Lusth/Vampire A Lusth.dae", false, "backpack");

    ResourceManager::LoadAnimation("resources/animation/Capoeira.dae", ResourceManager::GetModel("backpack"), "dance");
    GameObject object(ResourceManager::GetModel("backpack"), glm::vec3(0.0f, -0.4f, 0.0f), glm::vec3(1.0f));

    object.Animator3D.PlayAnimation(&ResourceManager::GetAnimation("dance"));

    ResourceManager::LoadTerrain("resources/texture/Diffuse_16BIT_PNG.png", "resources/texture/Heightmap_16BIT_PNG.png", "terrain1", 4096.0f, 3.0f, 128);
    Chunk chunk(0, 0, ResourceManager::GetTerrain("terrain1"));

    ChunkGrid chunkGrid(2);

    int n = chunkGrid.GetGridSize();
    for(int z = 0; z < n; z++)
    {
        for(int x = 0; x < n ; x++)
        {
            chunkGrid.SetupChunkTerrain(x, z, ResourceManager::GetTerrain("terrain1"));
            chunkGrid.GetChunk(x, z)->ChunkTransform.scale = glm::vec3(x % 2 == 0 ? 1.0f : -1.0f, 1.0f, z % 2 == 0 ? -1.0f : 1.0f);
        }
    }

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    camera.movementSpeed = 1000.0f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        Chunk* currentChunk = chunkGrid.GetChunk(camera.cameraPos.x, camera.cameraPos.z);
        if(currentChunk)
        {
            float targetY  = currentChunk->GetWorldHeight(camera.cameraPos.x, camera.cameraPos.z) + 64.0f;

            camera.cameraPos.y = glm::mix(
                camera.cameraPos.y,
                targetY,
                deltaTime * smoothSpeed
            );
        }

        glm::ivec2 currentChunkCoords = chunkGrid.getChunkCoordsFromWorldPos(camera.cameraPos.x, camera.cameraPos.z);
        std::cout << "chunkX: " << currentChunkCoords.x << " chunkZ: " << currentChunkCoords.y << std::endl;

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ResourceManager::GetShader("modelLoading").SetMatrix4("projection", projection);
        ResourceManager::GetShader("modelLoading").SetMatrix4("view", view);

        ResourceManager::GetShader("terrainShader").SetMatrix4("projection", projection);
        ResourceManager::GetShader("terrainShader").SetMatrix4("view", view);

        chunkGrid.Draw(ResourceManager::GetShader("terrainShader"));

        // render the loaded model
        //Renderer.Draw(ResourceManager::GetShader("modelLoading"), object, deltaTime);
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)

        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}