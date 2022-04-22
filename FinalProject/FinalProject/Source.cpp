#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera class

using namespace std; // Standard namespace

static int perspective_state = 1; // Ortho view = 0, Perspective = 1
/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Final Project"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint tablevao;         // Handle for the table vertex array object
        GLuint bowlvao;// Handle for the bowl vertex array object
        GLuint plantarvao;      //Handle for the plantar vertex array object
        GLuint dirtvao;      //Handle for the dirt vertex array object
        GLuint grindervao;      //Handle for the grinder vertex array object
        GLuint tablevbo;     // Handles for the table vertex buffer objects
        GLuint bowlvbo;     // Handles for the bowl vertex buffer objects
        GLuint plantarvbo;  // Handles for the plantar vertex buffer objects
        GLuint dirtvbo;  // Handles for the dirt vertex buffer objects
        GLuint grindervbo;  // Handles for the grinder vertex buffer objects
        GLuint nTableVerticies;    // Number of verts of the table mesh
        GLuint nBowlVerticies;    // Number of verts of the bowl mesh
        GLuint nPlantarVerticies;   // Number of verts for the plantar mesh
        GLuint nDirtVerticies;     // Number of verts for the dirt mesh
        GLuint nGrinderVerticies;  // Number of verts for the grinder mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture id
    GLuint gTextureTableId;
    GLuint gTextureBowlId;
    GLuint gTexturePlantarId;
    GLuint gTextureDirtId;
    GLuint gTextureGrinderId;
    glm::vec2 gUVScale(1.0f, 1.0f);
    // Shader program
    GLuint gTableProgramId;
    GLuint gBowlProgramId;
    GLuint gPlantarProgramId;
    GLuint gDirtProgramId;
    GLuint gGrinderProgramId;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Subject position and scale
    glm::vec3 gTablePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gTableScale(1.0f);

    glm::vec3 gBowlPosition(-0.7f, 0.0f, 0.0f);
    glm::vec3 gBowlScale(1.0f);

    glm::vec3 gPlantarPosition(1.0, 0.0f, 0.0f);
    glm::vec3 gPlantarScale(1.0f);

    glm::vec3 gDirtPosition(1.0, 0.0f, 0.0f);
    glm::vec3 gDirtScale(1.0f);

    glm::vec3 gGrinderPosition(-0.7f, 0.0f, 0.0f);
    glm::vec3 gGrinderScale(1.0f);


    // Cube and light color

    glm::vec3 gObjectColor(1.0f, 0.2f, 0.0f);
    glm::vec3 gKeyLightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 gSpotLightColor(0.0f, 0.0f, 1.0f);


    // Light position and scale
    glm::vec3 gKeyLightPosition(-1.5f, 3.5f, 0.0f);
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::vec3 gSpotLightPosition = gCamera.Position;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateTableMesh(GLMesh& mesh);
void UCreateBowlMesh(GLMesh& mesh);
void UCreateDirtMesh(GLMesh& mesh);
void UCreatePlantarMesh(GLMesh& mesh);
void UCreateGrinderMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
glm::vec3 CalculateSurfaceNormal(glm::vec3 vecOne, glm::vec3 vecTwo, glm::vec3 vecThree);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;


//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;
out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 keyLightColor;
uniform vec3 keyLightPos;
uniform vec3 spotLightPos;
uniform vec3 spotLightColor;
uniform vec3 viewPosition;
uniform vec3 spotLightDirection;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.1f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * keyLightColor; // Generate ambient light color

    // calculate lighting for keylight
    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(keyLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * keyLightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * keyLightColor;

    // calclate lighting for spotlight
    
    float cutOff = cos(radians(8.5f));
    float outerCutOff = cos(radians(12.5f));
    ////Calculate Ambient lighting*/

   

    ////Calculate Diffuse lighting*/

    lightDirection = normalize(spotLightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    impact = max(dot(norm, lightDirection), 1.0);// Calculate diffuse impact by generating dot product of normal and light
    //diffuse = diffuse + impact *
        //spotLightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    specularIntensity = 1.0f; // Set specular light strength
    highlightSize = 16.0f; // Set specular highlight size
    viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
   

    // spotlight code
    float theta = dot(lightDirection, normalize(-spotLightDirection));
    float epsilon = (cutOff - outerCutOff);
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    ambient = ambient + ambientStrength * spotLightColor * intensity;
    specular = specular + specularIntensity * specularComponent * spotLightColor * intensity;
    diffuse = diffuse + impact * spotLightColor * intensity; // Generate diffuse light color
    //

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0f); // Send lighting results to GPU
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateTableMesh(gMesh); // Calls the function to create the table mesh
    UCreateBowlMesh(gMesh); // calls the function to create the bowl mesh
    UCreatePlantarMesh(gMesh);  //calls the function to create the plantar mesh
    UCreateDirtMesh(gMesh);  //calls the function to create the dirt mesh
    UCreateGrinderMesh(gMesh);  //calls the function to create the dirt mesh

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gTableProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gBowlProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gPlantarProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gDirtProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gGrinderProgramId))
        return EXIT_FAILURE;

    // Load texture
    const char* texFilename = "../resources/textures/old_wood.jpg";
    if (!UCreateTexture(texFilename, gTextureTableId))
        return EXIT_FAILURE;
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gTableProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gTableProgramId, "uTexture"), 0);

    // Load texture
    texFilename = "../resources/textures/stone_rock.jpg";
    if (!UCreateTexture(texFilename, gTextureBowlId))
        return EXIT_FAILURE;
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gBowlProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gBowlProgramId, "uTexture"), 0);

    // Load texture
    texFilename = "../resources/textures/granite.jpg";
    if (!UCreateTexture(texFilename, gTextureGrinderId))
        return EXIT_FAILURE;
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gGrinderProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gGrinderProgramId, "uTexture"), 0);

    // Load texture
    texFilename = "../resources/textures/pot2.jpg";
    if (!UCreateTexture(texFilename, gTexturePlantarId))
        return EXIT_FAILURE;
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gPlantarProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gPlantarProgramId, "uTexture"), 0);

    // Load texture
    texFilename = "../resources/textures/plantar_dirt.jpg";
    if (!UCreateTexture(texFilename, gTextureDirtId))
        return EXIT_FAILURE;
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gDirtProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gDirtProgramId, "uTexture"), 0);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTextureTableId);
    UDestroyTexture(gTextureBowlId);
    UDestroyTexture(gTexturePlantarId);
    UDestroyTexture(gTextureDirtId);
    UDestroyTexture(gTextureGrinderId);

    // Release shader program
    UDestroyShaderProgram(gTableProgramId);
    UDestroyShaderProgram(gBowlProgramId);
    UDestroyShaderProgram(gPlantarProgramId);
    UDestroyShaderProgram(gDirtProgramId);
    UDestroyShaderProgram(gGrinderProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // monitor key press to move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);  // added enum to camera class
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);  // added enum to camera class

    gSpotLightPosition = gCamera.Position;

}

// handles single key press so that perspective/ortho can be changed only once on p press
void USingleKeyPressCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        // change the state to the opposite of current state
        perspective_state = abs(perspective_state - 1);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    if (perspective_state) {

        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, .1f, 100.0f);
    }

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.tablevao);
    // Set the shader to be used
    glUseProgram(gTableProgramId);

    model = glm::translate(gTablePosition) * glm::scale(gTableScale);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gTableProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gTableProgramId, "view");
    GLint projLoc = glGetUniformLocation(gTableProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gTableProgramId, "objectColor");
    GLint keyLightColorLoc = glGetUniformLocation(gTableProgramId, "keyLightColor");
    GLint keyLightPositionLoc = glGetUniformLocation(gTableProgramId, "keyLightPos");
    GLint viewPositionLoc = glGetUniformLocation(gTableProgramId, "viewPosition");
    GLint spotLightColorLoc = glGetUniformLocation(gTableProgramId, "spotLightColor");
    GLint spotLightPositionLoc = glGetUniformLocation(gTableProgramId, "spotLightPos");
    GLint spotLightDirectionLoc = glGetUniformLocation(gTableProgramId, "spotLightDirection");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(keyLightColorLoc, gKeyLightColor.r, gKeyLightColor.g, gKeyLightColor.b);
    glUniform3f(keyLightPositionLoc, gKeyLightPosition.x, gKeyLightPosition.y, gKeyLightPosition.z);
    glUniform3f(spotLightColorLoc, gSpotLightColor.r, gSpotLightColor.g, gSpotLightColor.b);
    glUniform3f(spotLightPositionLoc,gSpotLightPosition.x, gSpotLightPosition.y, gSpotLightPosition.z);
    glUniform3f(spotLightDirectionLoc, gCamera.Front.x, gCamera.Front.y, gCamera.Front.z);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gTableProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));



    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureTableId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nTableVerticies);


    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.bowlvao);

    // Set the shader to be used
    glUseProgram(gBowlProgramId);

    model = glm::translate(gBowlPosition) * glm::scale(gBowlScale);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gBowlProgramId, "model");
    viewLoc = glGetUniformLocation(gBowlProgramId, "view");
    projLoc = glGetUniformLocation(gBowlProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the light color, light position, and camera position

    keyLightColorLoc = glGetUniformLocation(gBowlProgramId, "keyLightColor");
    keyLightPositionLoc = glGetUniformLocation(gBowlProgramId, "keyLightPos");
    viewPositionLoc = glGetUniformLocation(gBowlProgramId, "viewPosition");
    spotLightColorLoc = glGetUniformLocation(gBowlProgramId, "spotLightColor");
    spotLightPositionLoc = glGetUniformLocation(gBowlProgramId, "spotLightPos");
    spotLightDirectionLoc = glGetUniformLocation(gBowlProgramId, "spotLightDirection");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(keyLightColorLoc, gKeyLightColor.r, gKeyLightColor.g, gKeyLightColor.b);
    glUniform3f(keyLightPositionLoc, gKeyLightPosition.x, gKeyLightPosition.y, gKeyLightPosition.z);
    glUniform3f(spotLightColorLoc, gSpotLightColor.r, gSpotLightColor.g, gSpotLightColor.b);
    glUniform3f(spotLightPositionLoc, gSpotLightPosition.x, gSpotLightPosition.y, gSpotLightPosition.z);
    glUniform3f(spotLightDirectionLoc, gCamera.Front.x, gCamera.Front.y, gCamera.Front.z);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gBowlProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureBowlId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nBowlVerticies);


    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.grindervao);

    // Set the shader to be used
    glUseProgram(gGrinderProgramId);

    model = glm::translate(gGrinderPosition) * glm::scale(gGrinderScale);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gGrinderProgramId, "model");
    viewLoc = glGetUniformLocation(gGrinderProgramId, "view");
    projLoc = glGetUniformLocation(gGrinderProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the light color, light position, and camera position

    keyLightColorLoc = glGetUniformLocation(gGrinderProgramId, "keyLightColor");
    keyLightPositionLoc = glGetUniformLocation(gGrinderProgramId, "keyLightPos");
    viewPositionLoc = glGetUniformLocation(gGrinderProgramId, "viewPosition");
    spotLightColorLoc = glGetUniformLocation(gGrinderProgramId, "spotLightColor");
    spotLightPositionLoc = glGetUniformLocation(gGrinderProgramId, "spotLightPos");
    spotLightDirectionLoc = glGetUniformLocation(gGrinderProgramId, "spotLightDirection");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(keyLightColorLoc, gKeyLightColor.r, gKeyLightColor.g, gKeyLightColor.b);
    glUniform3f(keyLightPositionLoc, gKeyLightPosition.x, gKeyLightPosition.y, gKeyLightPosition.z);
    glUniform3f(spotLightColorLoc, gSpotLightColor.r, gSpotLightColor.g, gSpotLightColor.b);
    glUniform3f(spotLightPositionLoc, gSpotLightPosition.x, gSpotLightPosition.y, gSpotLightPosition.z);
    glUniform3f(spotLightDirectionLoc, gCamera.Front.x, gCamera.Front.y, gCamera.Front.z);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gGrinderProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureGrinderId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nGrinderVerticies);

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.plantarvao);

    // Set the shader to be used
    glUseProgram(gPlantarProgramId);

    model = glm::translate(gPlantarPosition) * glm::scale(gPlantarScale);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gPlantarProgramId, "model");
    viewLoc = glGetUniformLocation(gPlantarProgramId, "view");
    projLoc = glGetUniformLocation(gPlantarProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the light color, light position, and camera position

    keyLightColorLoc = glGetUniformLocation(gPlantarProgramId, "keyLightColor");
    keyLightPositionLoc = glGetUniformLocation(gPlantarProgramId, "keyLightPos");
    spotLightColorLoc = glGetUniformLocation(gPlantarProgramId, "spotLightColor");
    spotLightPositionLoc = glGetUniformLocation(gPlantarProgramId, "spotLightPos");
    spotLightDirectionLoc = glGetUniformLocation(gPlantarProgramId, "spotLightDirection");
    viewPositionLoc = glGetUniformLocation(gPlantarProgramId, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(keyLightColorLoc, gKeyLightColor.r, gKeyLightColor.g, gKeyLightColor.b);
    glUniform3f(keyLightPositionLoc, gKeyLightPosition.x, gKeyLightPosition.y, gKeyLightPosition.z);
    glUniform3f(spotLightColorLoc, gSpotLightColor.r, gSpotLightColor.g, gSpotLightColor.b);
    glUniform3f(spotLightPositionLoc, gSpotLightPosition.x, gSpotLightPosition.y, gSpotLightPosition.z);
    glUniform3f(spotLightDirectionLoc, gCamera.Front.x, gCamera.Front.y, gCamera.Front.z);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gPlantarProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexturePlantarId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nPlantarVerticies);

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.dirtvao);

    // Set the shader to be used
    glUseProgram(gDirtProgramId);

    model = glm::translate(gDirtPosition) * glm::scale(gDirtScale);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gDirtProgramId, "model");
    viewLoc = glGetUniformLocation(gDirtProgramId, "view");
    projLoc = glGetUniformLocation(gDirtProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    keyLightColorLoc = glGetUniformLocation(gDirtProgramId, "keyLightColor");
    keyLightPositionLoc = glGetUniformLocation(gDirtProgramId, "keyLightPos");
    spotLightColorLoc = glGetUniformLocation(gDirtProgramId, "spotLightColor");
    spotLightPositionLoc = glGetUniformLocation(gDirtProgramId, "spotLightPos");
    spotLightDirectionLoc = glGetUniformLocation(gDirtProgramId, "spotLightDirection");
    viewPositionLoc = glGetUniformLocation(gDirtProgramId, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(keyLightColorLoc, gKeyLightColor.r, gKeyLightColor.g, gKeyLightColor.b);
    glUniform3f(keyLightPositionLoc, gKeyLightPosition.x, gKeyLightPosition.y, gKeyLightPosition.z);
    glUniform3f(spotLightColorLoc, gSpotLightColor.r, gSpotLightColor.g, gSpotLightColor.b);
    glUniform3f(spotLightPositionLoc, gSpotLightPosition.x, gSpotLightPosition.y, gSpotLightPosition.z);
    glUniform3f(spotLightDirectionLoc, gCamera.Front.x, gCamera.Front.y, gCamera.Front.z);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(gDirtProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureDirtId);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nDirtVerticies);


    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// creates the vertices from one circle to another with texture points
void getUnitCircleVertices(vector<GLfloat>& verts, GLint sectorCount, GLfloat firstYCoord, GLfloat secondYCoord, GLfloat firstRadius, GLfloat secondRadius)
{
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / sectorCount;
    float sectorAngle;  // radian
    float x, z;  // variables for position of verticies
    float topV = sqrt(pow(firstYCoord - secondYCoord, 2) + pow((firstRadius - secondRadius),2));
    float bottomV = 0.0;
    glm::vec3 normal;
    glm::vec3 firstVec(0.0f);
    glm::vec3 secondVec(0.0f);
    glm::vec3 thirdVec(0.0f);

    for (int i = 0; i < sectorCount; ++i)
    {
        // calculate the vertex for ith point in first cirlce
        sectorAngle = i * (sectorStep);
        x = firstRadius * cos(sectorAngle);
        z = (firstRadius * sin(sectorAngle));
        // push the x y and z coordinates into verts
        verts.push_back(x); // x
        firstVec.x = x;
        verts.push_back(firstYCoord); // y
        firstVec.y = firstYCoord;
        verts.push_back(z); // z
        firstVec.z = z;
        // push 3 numbers into verts as holder for the normal
        verts.push_back(1.0);
        verts.push_back(1.0);
        verts.push_back(1.0);

        // push texture map coordinates into vert
        verts.push_back(sectorAngle/ (2*PI));
        verts.push_back(topV);

        // repeat same steps for ith point in second circle
        x = secondRadius * cos(sectorAngle);
        z = (secondRadius * sin(sectorAngle));
        verts.push_back(x); // x
        secondVec.x = x;
        verts.push_back(secondYCoord);                // y
        secondVec.y = secondYCoord;
        verts.push_back(z); // z
        secondVec.z = z;
        verts.push_back(0.0);
        verts.push_back(0.0);
        verts.push_back(0.0);

        verts.push_back(sectorAngle / (2 * PI));
        verts.push_back(bottomV);

        // repeat same steps for i + 1 point in second circle
        sectorAngle = (i + 1.0) * (sectorStep);

        x = secondRadius * cos(sectorAngle);
        z = (secondRadius * sin(sectorAngle));
        verts.push_back(x); // x
        thirdVec.x = x;
        verts.push_back(secondYCoord);                // y
        thirdVec.y = secondYCoord;
        verts.push_back(z); // z
        thirdVec.z = z;
        // we now have all three points of the triangle and can calculate the normal vector
        normal = CalculateSurfaceNormal(firstVec, secondVec, thirdVec);
        //push the normal vector x, y, and z into verts
        verts.push_back(normal.x * -1);
        verts.push_back(normal.y);
        verts.push_back(normal.z * -1);

        verts.push_back(sectorAngle / (2 * PI));
        verts.push_back(bottomV);

        // replace all normal vector place holders with actual values
        verts.at(verts.size() - 21) = normal.x * -1;
        verts.at(verts.size() - 20) = normal.y;
        verts.at(verts.size() - 19) = normal.z * -1;

        verts.at(verts.size() - 13) = normal.x * -1;
        verts.at(verts.size() - 12) = normal.y;
        verts.at(verts.size() - 11) = normal.z * -1;

        // repeat all steps for second triangle to make up first section of final shape
        verts.push_back(x); // x
        firstVec.x = x;
        verts.push_back(secondYCoord);                // y
        firstVec.y = secondYCoord;
        verts.push_back(z); // z
        firstVec.z = z;

        verts.push_back(1.0);
        verts.push_back(1.0);
        verts.push_back(1.0);

        verts.push_back(sectorAngle / (2 * PI));
        verts.push_back(bottomV);

        x = firstRadius * cos(sectorAngle);
        z = (firstRadius * sin(sectorAngle));
        verts.push_back(x); // x
        secondVec.x = x;
        verts.push_back(firstYCoord);                // y
        secondVec.y = firstYCoord;
        verts.push_back(z); // z
        secondVec.z = z;

        verts.push_back(1.0);
        verts.push_back(1.0);
        verts.push_back(1.0);

        verts.push_back(sectorAngle / (2 * PI));
        verts.push_back(topV);

        sectorAngle = i * (sectorStep);
        x = firstRadius * cos(sectorAngle);
        z = (firstRadius * sin(sectorAngle));
        verts.push_back(x); // x
        thirdVec.x = x;
        verts.push_back(firstYCoord);                // y
        thirdVec.y = firstYCoord;
        verts.push_back(z); // z
        thirdVec.z = z;
        normal = CalculateSurfaceNormal(firstVec, secondVec, thirdVec);
        verts.push_back(normal.x * -1);
        verts.push_back(normal.y);
        verts.push_back(normal.z * -1);

        verts.push_back(sectorAngle / (2 * PI));
        verts.push_back(topV);


        verts.at(verts.size() - 21) = normal.x * -1;
        verts.at(verts.size() - 20) = normal.y;
        verts.at(verts.size() - 19) = normal.z * -1;

        verts.at(verts.size() - 13) = normal.x * -1;
        verts.at(verts.size() - 12) = normal.y;
        verts.at(verts.size() - 11) = normal.z * -1;
    }
}

// generate vertices for +X face only by intersecting 2 circular planes
// (longitudinal and latitudinal) at the given longitude/latitude angles
void buildUnitPositiveX(vector<GLfloat> verts, int subdivision)
{
    const float DEG2RAD = acos(-1) / 180.0f;

    std::vector<float> vertices;
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis
    //gsl::

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for (unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for (unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            verts.push_back(v[0]);
            verts.push_back(v[1]);
            verts.push_back(v[2]);
        }
    }

}



// Implements the UCreateMesh function to create the bowl
void UCreateBowlMesh(GLMesh& mesh)
{
    GLint numberOfVerts = 60;
    // Specifies Normalized Device Coordinates (x,y,z) and image (u,v) for triangle vertices
    vector<GLfloat> verts;


    // create verticies for bowl  
    getUnitCircleVertices(verts, numberOfVerts, -0.2, -0.2, 0.5, 0.4);
    getUnitCircleVertices(verts, numberOfVerts, -0.2, -0.4, 0.4, 0.0);
    getUnitCircleVertices(verts, numberOfVerts, -0.2, -0.49, 0.5, 0.4);
    getUnitCircleVertices(verts, numberOfVerts, -0.49, -0.49, 0.4, 0.0);

    cout << verts.size();

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nBowlVerticies = verts.size() / (floatsPerVertex + floatsPerNormal + floatsPerUV);

    cout << mesh.nBowlVerticies;

    glGenVertexArrays(1, &mesh.bowlvao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.bowlvao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.bowlvbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.bowlvbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), &verts.front(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU


    // Strides between vertex coordinates is 6 (x, y, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Creates the Vertex Attribute Pointer
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);


}

// Implements the UCreateMesh function to create the grinder
void UCreateGrinderMesh(GLMesh& mesh)
{
    GLint numberOfVerts = 60;
    // Specifies Normalized Device Coordinates (x,y,z) and image (u,v) for triangle vertices
    vector<GLfloat> verts;


    // create verticies for Grinder  
    //sides of grinder
   getUnitCircleVertices(verts, numberOfVerts, -0.2, -0.4, 0.05, 0.06);
   // top of grinder
   getUnitCircleVertices(verts, numberOfVerts, -0.2, -0.2, 0.05, 0);
    // bottom of grinder
   getUnitCircleVertices(verts, numberOfVerts, -0.4, -0.4, 0.06, 0);
    
   

    cout << verts.size();

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nGrinderVerticies = verts.size() / (floatsPerVertex + floatsPerNormal + floatsPerUV);

    cout << mesh.nGrinderVerticies;

    glGenVertexArrays(1, &mesh.grindervao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.grindervao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.grindervbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.grindervbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), &verts.front(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU


    // Strides between vertex coordinates is 6 (x, y, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Creates the Vertex Attribute Pointer
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);


}


// Implements the UCreateMesh function
void UCreateTableMesh(GLMesh& mesh)
{
    // Vertex data
    vector<GLfloat> verts = {
        // The two triangles will be drawn using indices
        // Left triangle indices: 0, 1, 2
        // Right triangle indices: 3, 2, 4

        -2.0f, -0.5f, -2.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        2.0f, -0.5f, -2.0f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -2.0f, -0.5f, 2.0f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top left


        2.0f, -0.5f, -2.0f,     0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -2.0f, -0.5f, 2.0f,     0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
        2.0f, -0.5f, 2.0f,        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right

         -2.0f, -0.51f, -2.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        2.0f, -0.51f, -2.0f,    0.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -2.0f, -0.51f, 2.0f,    0.0f, -1.0f, 0.0f,  0.0f, 1.0f, // top left


        2.0f, -0.51f, -2.0f,     0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -2.0f, -0.51f, 2.0f,     0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top left
        2.0f, -0.51f, 2.0f,        0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top right
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nTableVerticies = verts.size() / (floatsPerVertex + floatsPerNormal + floatsPerUV);

    glGenVertexArrays(1, &mesh.tablevao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.tablevao);

    // Create VBO
    glGenBuffers(1, &mesh.tablevbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.tablevbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), &verts.front(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU


   // Strides between vertex coordinates is 6 (x, y, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Creates the Vertex Attribute Pointer
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

void UCreatePlantarMesh(GLMesh& mesh) {
    vector<GLfloat> verts = {
        //plantar back face
          -0.15f, -0.5f, -0.15f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.15f, -0.5f, -0.15f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.25f,  0.0f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.25f,  0.0f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.25f,  0.0f, -0.25f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.15f, -0.5f, -0.15f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        // plantar front face
        -0.15f, -0.5f,  0.15f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.15f, -0.5f,  0.15f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.25f,  0.0f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.25f,  0.0f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.25f,  0.0f,  0.25f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.15f, -0.5f,  0.15f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        // left face
        -0.25f,  0.0f,  0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.25f,  0.0f, -0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.15f, -0.5f, -0.15f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.15f, -0.5f, -0.15f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.15f, -0.5f,  0.15f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.25f,  0.0f,  0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        // right face
         0.25f,  0.0f,  0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.25f,  0.0f, -0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.15f, -0.5f, -0.15f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.15f, -0.5f, -0.15f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.15f, -0.5f,  0.15f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.25f,  0.0f,  0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         // bottom face
        -0.15f, -0.5f, -0.15f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.15f, -0.5f, -0.15f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.15f, -0.5f,  0.15f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.15f, -0.5f,  0.15f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.15f, -0.5f,  0.15f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.15f, -0.5f, -0.15f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        //top face
        -0.25f,  0.0f, -0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.25f,  0.0f, -0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.25f,  0.0f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.25f,  0.0f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.25f,  0.0f,  0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,  0.0f, -0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nPlantarVerticies = verts.size() / (floatsPerVertex + floatsPerNormal + floatsPerUV);


    glGenVertexArrays(1, &mesh.plantarvao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.plantarvao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.plantarvbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.plantarvbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), &verts.front(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU


    // Strides between vertex coordinates is 6 (x, y, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Creates the Vertex Attribute Pointer
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

}

void UCreateDirtMesh(GLMesh& mesh) {
    vector<GLfloat> verts = {
        //Dirt
          
        //top face
        -0.2f,  0.01f, -0.2f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.2f,  0.01f, -0.2f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.2f,  0.01f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.2f,  0.01f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.2f,  0.01f,  0.2f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.2f,  0.01f, -0.2f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nDirtVerticies = verts.size() / (floatsPerVertex + floatsPerNormal + floatsPerUV);


    glGenVertexArrays(1, &mesh.dirtvao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.dirtvao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.dirtvbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.dirtvbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), &verts.front(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU


    // Strides between vertex coordinates is 6 (x, y, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);// The number of floats before each

    // Creates the Vertex Attribute Pointer
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

}

// returns the normal of a triangle given the three vectors of the triangle
glm::vec3 CalculateSurfaceNormal(glm::vec3 vOne, glm::vec3 vTwo, glm::vec3 vThree) {
    glm::vec3 normal(0.0f, 0.0f, 0.0f);

    normal.x = normal.x + (vOne.y - vTwo.y) * (vOne.z + vTwo.z);
    normal.y = normal.y + (vOne.z - vTwo.z) * (vOne.x + vTwo.x);
    normal.z = normal.z + (vOne.x - vTwo.x) * (vOne.y + vTwo.y);

    normal.x = normal.x + (vTwo.y - vThree.y) * (vTwo.z + vThree.z);
    normal.y = normal.y + (vTwo.z - vThree.z) * (vTwo.x + vThree.x);
    normal.z = normal.z + (vTwo.x - vThree.x) * (vTwo.y + vThree.y);

    normal.x = normal.x + (vThree.y - vOne.y) * (vThree.z + vOne.z);
    normal.y = normal.y + (vThree.z - vOne.z) * (vThree.x + vOne.x);
    normal.z = normal.z + (vThree.x - vOne.x) * (vThree.y + vOne.y);

    return normal;

}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.tablevao);
    glDeleteBuffers(1, &mesh.tablevbo);
    glDeleteVertexArrays(1, &mesh.bowlvao);
    glDeleteBuffers(1, &mesh.bowlvbo);
    glDeleteVertexArrays(1, &mesh.plantarvao);
    glDeleteBuffers(1, &mesh.plantarvbo);
    glDeleteVertexArrays(1, &mesh.dirtvao);
    glDeleteBuffers(1, &mesh.dirtvbo);
    glDeleteVertexArrays(1, &mesh.grindervao);
    glDeleteBuffers(1, &mesh.grindervbo);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
