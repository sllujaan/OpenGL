#include"pch.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>


#include"vender/imgui/imgui.h"
#include"vender/imgui/imgui_impl_opengl3.h"

#include"Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//const GLchar* const* string;

#define NAME_TO_STRING(X) #X

int SHADER_GLOBAL = NULL;
int MOUSE_LEFT_REPEAT = 0;
float CURR_TRANSLATE_X = 0.0f;
float CURR_TRANSLATE_Y = 0.0f;
float CURR_TRANSLATE_Z = 0.0f;

glm::mat4 PROJECTION = glm::mat4(0.0f);
glm::mat4 VIEW = glm::mat4(0.0f);
glm::mat4 MODEL = glm::mat4(0.0f);
float FOV = 25.0f;
int ANIMATE_TRASLATE_ONKEY_W_S_A_D = false;

//glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)
//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

glm::vec3 CAMERA_POS = glm::vec3(0.0f, 0.0f, -10.0f);
glm::vec3 CAMERA_FRONT = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);

enum Zoom
{
    In,
    Out
};

typedef enum transDirection
{
    left,
    right,
    up,
    down
} rotateDir;


struct APP_THREAD_INFO
{
    int MAIN_THREAD_SOULD_CLOSE = 0;
    int ALL_CHILD_THREADS_SOULD_CLOSE = 0;
};

APP_THREAD_INFO _appThreadInfo;


//glm::mat4 trans = glm::mat4(1.0f);
//VIEW = glm::lookAt(CAMERA_POS, CAMERA_FRONT, CAMERA_UP);
//trans = PROJECTION * VIEW * MODEL;
//GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");
//glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));

template<class Fn, class... Args>
struct ShaderAnimateStruct {
    GLint vbLocation;
    Fn glUinformFN;
};


GLFWwindow* GLF_WINDOW;

void handle_key(GLFWwindow* window, int key, int scancode, int action, int mods);


glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model;
}




void animateShaderThread() {
    
    while (!_appThreadInfo.ALL_CHILD_THREADS_SOULD_CLOSE)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "animateShaderThread: " << _threadid << std::endl;

        if (ANIMATE_TRASLATE_ONKEY_W_S_A_D) {
            /*for (size_t i = 0; i < 20; i++)
            {*/
                //handle_key(GLF_WINDOW, 0, 0, 0, 0);
                glm::mat4 trans = glm::mat4(1.0f);
                VIEW = glm::lookAt(CAMERA_POS, CAMERA_FRONT, CAMERA_UP);
                trans = PROJECTION * VIEW * MODEL;
                GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");
                //glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            //}
            ANIMATE_TRASLATE_ONKEY_W_S_A_D = 0;
        }
        
    }
    return;
}



void translateShader(transDirection dir) {
    float translateX = 0.0f;
    float translateY = 0.0f;

    switch (dir)
    {
    case left:
        translateX -= 0.1f;
        break;
    case right:
        translateX += 0.1f;
        break;
    case up:
        translateY += 0.1f;
        break;
    case down:
        translateY -= 0.1f;
        break;
    default:
        break;
    }

    //pass trans camera to the shader
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");
    glm::mat4 trans(1.0f);
    glm::mat4 view(1.0f);
    float angle = 0.0f;
    //trans = camera(translate, glm::vec2(angle, angle));
    
    view = glm::translate(VIEW, glm::vec3(-translateX, -translateY, 0.0f));
    VIEW = view;
    trans = PROJECTION * MODEL * VIEW;
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
}


void rotateScene(rotateDir dir) {
    float rotateX = 0.0f;
    float rotateY = 0.0f;

    switch (dir)
    {
    case left:
        rotateX -= 0.1f;
        break;
    case right:
        rotateX += 0.1f;
        break;
    case up:
        rotateY += 0.1f;
        break;
    case down:
        rotateY -= 0.1f;
        break;
    default:
        break;
    }

    //pass trans camera to the shader
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");
    glm::mat4 View(1.0f);
    float angle = 0.0f;
    //View = camera(1.0f, glm::vec2(rotateX, rotateY));
    //View = glm::rotate(View, rotateX, glm::vec3(0.0f, 0.0f, 0.0f));
    //View = glm::rotate(View, rotateY, glm::vec3(0.0f, 0.0f, 0.0f));
    //View = VIEW * View;
    View = glm::rotate(VIEW, -rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, -rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
    VIEW = View;
    View = PROJECTION * MODEL * VIEW;
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(View));
}



void translateShaderCamera() {
    static float rotateX = 0.0f;
    static float rotateY = 0.0f;

    rotateX += 0.1f;
    rotateY += 0.1f;

    //pass view camera to the shader
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");
    glm::mat4 view(1.0f);
    view = camera(2, glm::vec2(rotateX, rotateY));
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(view));
}



void zoomInOutCamera(Zoom zoom) {

    float translate = 0.0f;
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");

    if (zoom == Zoom::In) {
        translate -= 0.5f;
    }
    else if (zoom == Zoom::Out) {
        translate += 0.5f;
    }
    //pass trans camera to the shader
    glm::mat4 trans(1.0f);
    float angle = 0.0f;
    //trans = camera(translate, glm::vec2(angle, angle));

    glm::mat4 Projection = glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, 100.0f);
    PROJECTION = Projection;
    trans = PROJECTION * MODEL * VIEW;

    
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
}


bool firstMouse = false;
float lastX = 400, lastY = 300;
float yaw = 0.0f;
float pitch = 0.0f;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {


    std::cout << std::this_thread::get_id() << std::endl;

    //std::cout << "mouse_callback" << std::endl;
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");


    if (MOUSE_LEFT_REPEAT) {
        std::cout << "mouse_callback translate the scene" << std::endl;
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 1.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;


        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        CAMERA_FRONT = glm::normalize(direction);


        glm::mat4 trans = glm::mat4(1.0f);
        VIEW = glm::lookAt(CAMERA_POS, CAMERA_FRONT, CAMERA_UP);
        trans = PROJECTION * VIEW * MODEL;
        glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
        

        return;
        //----------------------------------------------------------------
        

        //std::cout << xoffset << ", " << yoffset << std::endl;
        //
        //glm::mat4 view = glm::translate(VIEW, glm::vec3(xoffset, yoffset, 0.0f));
        //VIEW = view;
        //glm::mat4 trans = PROJECTION * MODEL * VIEW;
        ////glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(xoffset, yoffset, 0.0f));
        //glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
        
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //std::cout << _threadid << std::endl;

    FOV -= (float)yoffset;
    if (FOV < 1.0f)
        FOV = 1.0f;
    if (FOV > 45.0f)
        FOV = 45.0f;

    std::cout << xoffset << std::endl;
    
    std::cout << yoffset << std::endl;
    if (yoffset > 0) {
        //mouse scroll up
        zoomInOutCamera(Zoom::In);
    }
    else {
        //mouse scroll up
        zoomInOutCamera(Zoom::Out);
    }

}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "mouse left click" << std::endl;
        MOUSE_LEFT_REPEAT = 1;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        std::cout << "mouse left click release" << std::endl;
        MOUSE_LEFT_REPEAT = 0;
    }

}

void handle_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float cameraSpeed = 0.05f;
    static size_t countPerKey = 0;


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        CAMERA_POS.y += cameraSpeed;
        CAMERA_FRONT.y += cameraSpeed;
        CAMERA_UP.y += CAMERA_FRONT.y;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        CAMERA_POS.y -= cameraSpeed;
        CAMERA_FRONT.y -= cameraSpeed;
        CAMERA_UP.y -= CAMERA_FRONT.y;

    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        CAMERA_POS.x += cameraSpeed;
        CAMERA_FRONT.x += cameraSpeed;
        //CAMERA_UP.x -= cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        CAMERA_POS.x -= cameraSpeed;
        CAMERA_FRONT.x -= cameraSpeed;
        //CAMERA_UP.x -= cameraSpeed;
    }


}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    handle_key(window, 0, 0, 0, 0);
    
    //GLF_WINDOW = window;
    //ANIMATE_TRASLATE_ONKEY_W_S_A_D = 1;
    float fov = 30.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        std::cout << "key up" << std::endl;
        yaw += 10.0f;
        //pitch += 0.1f;
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        CAMERA_FRONT = glm::normalize(direction);
    }

    glm::mat4 trans = glm::mat4(1.0f);
    VIEW = glm::lookAt(CAMERA_POS, CAMERA_FRONT, CAMERA_UP);
    trans = PROJECTION * VIEW * MODEL;
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));

  
}


static unsigned int compileShader(unsigned int type, std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(sizeof(char) * length);
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile: " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
            std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int createShader(std::string& vertexShader, std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDetachShader(program, vs);
    glDetachShader(program, fs);

    return program;
}

float getColor(float color);
float increaseColor(float color);
float decreaseColor(float color);



errno_t generateCirclePoints(std::vector<glm::vec3>& vbPoints) {
    float angle = 0.0f;
    const float ANGLE_INCREMENT = 90.0f;
    const float RADIUS = 0.5f;
    while (angle <= 360)
    {
        float x = RADIUS * cos(angle);
        float y = RADIUS * sin(angle);
        x;
        y;
        vbPoints.push_back(glm::vec3(x, y, 0));
        angle += ANGLE_INCREMENT;
    }

    return 0;
}



int main(void) {

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    std::cout << "_threadid main: " <<_threadid << std::endl;
    //thread
    std::thread th1(animateShaderThread);

    std::cout << std::this_thread::get_id() << std::endl;

    if (glewInit() != GLEW_OK)
        std::cout << "GLEW_ERROR" << std::endl;


    float vbo[] = {
        // positions            //color                //normals
       -1.2f, -1.1f,  1.0f,      1.0f, 0.0f, 0.0f,     -1.2f, -1.2f, -1.0f,   //left bottom
       -1.2f,  1.1f,  1.0f,      1.0f, 1.0f, 0.0f,     -1.2f,  1.2f, -1.0f,   //left top
        1.2f,  1.1f,  1.0f,      1.0f, 1.0f, 0.0f,      1.2f,  1.2f, -1.0f,   //right top
        1.2f, -1.1f,  1.0f,      1.0f, 1.0f, 0.0f,      1.2f, -1.2f, -1.0f,   //right bottom

        // positions            //color                //normals
       -0.2f, -0.2f, -1.0f,      1.0f, 0.0f, 1.0f,     -1.2f, -1.2f, -1.0f,   //left bottom
       -0.2f,  0.2f, -1.0f,      1.0f, 0.0f, 0.0f,     -1.2f,  1.2f, -1.0f,   //left top
        0.2f,  0.2f, -1.0f,      1.0f, 0.0f, 0.0f,      1.2f,  1.2f, -1.0f,   //right top
        0.2f, -0.2f, -1.0f,      1.0f, 0.0f, 0.0f,      1.2f, -1.2f, -1.0f,   //right bottom

    };


    /*
      // positions            //color                //normals
        -0.9f, -0.9f,  1.0f,     1.0f, 0.5f, 1.0f,     -0.9f, -0.5f, -1.0f,   //left bottom
        -0.9f,  0.9f,  1.0f,     1.0f, 0.5f, 1.0f,     -0.5f,  0.5f, -1.0f,   //left top
         0.9f,  0.9f,  1.0f,     1.0f, 0.5f, 1.0f,      0.5f,  0.5f, -1.0f,   //right top
         0.9f, -0.9f,  1.0f,     1.0f, 0.5f, 1.0f,      0.5f, -0.5f, -1.0f,   //right bottom

         // positions            //color                //normals
        -0.2f, -0.2f,  0.0f,      1.0f, 1.0f, 0.0f,     -0.5f, -0.5f, -1.0f,   //left bottom
        -0.2f,  0.2f,  0.0f,      1.0f, 1.0f, 0.0f,     -0.5f,  0.5f, -1.0f,   //left top
         0.2f,  0.2f,  0.0f,      1.0f, 1.0f, 0.0f,      0.5f,  0.5f, -1.0f,   //right top
         0.2f, -0.2f,  0.0f,      1.0f, 1.0f, 0.0f,      0.5f, -0.5f, -1.0f,   //right bottom

    */


    

    int bufferSize = (sizeof(vbo) / sizeof(vbo[0])) * sizeof(float);


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);

    /*triangle 1 ----------------*/
    //give buffer data to opengl
    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, vbo, GL_STATIC_DRAW);
    
    //quad buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color buffer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //normals binding.
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);




    //---------------------------------------Texture------------------------------------

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // load and generate the texture
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("C:\\Users\\SALMAN-ALTAF\\Downloads\\container2.png", &width, &height, &nrChannels, 0);

    if (data)
    {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //texture coordinates.
    /*glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);*/

    //---------------------------------------------------------------------------

    //normals binding.
    /*glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);*/



    std::string vertexShader =
        "#version 400\r\n"

        "layout (location = 0) in vec3 vp;"
        "layout(location = 1) in vec3 vertexColor;"
        "layout (location = 2) in vec3 aNormal;"


        "out vec3 fragmentColor;"
        "out vec2 TexCoord;"
        "out vec3 FragPos;"
        "out vec3 Normal;"

        "uniform mat4 trans;"
        "uniform mat4 lightSpaceMatrix;"

        "void main() {"
        "   vec3 offsets[2] = {vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f,  0.5f, 0.5f)};"
        //"   vec3 offset = offsets[gl_InstanceID];"
        "   gl_Position = trans * vec4(vp, 1.0);"
        "   "
        "   fragmentColor = vertexColor;"
        "   Normal = aNormal;"
        "   FragPos = vp;"
        "}";
    /*gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
    
    //gl_Position = trans * vec4(vp, 1.0);
    //gl_Position = vec4(vec2(FragPos) + offset, 0.0, 1.0);
    vec2 offsets[2] = {vec2(0.5f,  0.5f), vec2(-0.5f,  0.5f)};
    uniform vec2 offsets[2];
    */
    

    std::string fragmentShader =
        "#version 400\r\n"

        "in vec3 FragPos;"
        "in vec3 fragmentColor;"
        "in vec2 TexCoord;"
        "in vec3 Normal;"

        "out vec4 color;"

        "uniform sampler2D ourTexture;"
        "uniform vec4 newColor;"

        "void main() {"
        "   vec3 lightColor = vec3(1.0, 1.0, 0.0);"
        "   vec3 objectColor = fragmentColor;"
        "   vec3 lightPos = vec3(0.0, 0.0, -100.0);"
        "   vec3 viewPos = vec3(1.3, 1.3, -100.0);"
        "   float ambientStrength = 0.1;"
        "   vec3 ambient = ambientStrength * lightColor;"
        "   vec3 norm = normalize(Normal);"
        "   vec3 lightDir = normalize(lightPos - FragPos);"

        "   float diff = max(dot(norm, lightDir), 0.0);"
        "   vec3 diffuse = diff * lightColor;"

        "   float specularStrength = 0.5;"
        "   vec3 viewDir = normalize(viewPos - FragPos);"
        "   vec3 reflectDir = reflect(-lightDir, norm);"
        
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 200);"
        "   vec3 specular = specularStrength * spec * lightColor;"

        "   vec3 result = (ambient + diffuse + specular) * objectColor;"
        ""
        "   color = vec4(fragmentColor, 1.0f);"
        "}";

    //"  frag_colour = vec4(1, 0.0, 0.0, 1.0);"
    //"  color = vec4(fragmentColor, 1.0);"
    //"   color = vec4(fragmentColor, 1.0);"
    //"   color = texture(ourTexture, TexCoord);"

    unsigned int shader = createShader(vertexShader, fragmentShader);

    glUseProgram(shader);
    SHADER_GLOBAL = shader;

    //pass trans camera to the shader
    GLint vbLocation = glGetUniformLocation(shader, "trans");
    GLint vbOffsets = glGetUniformLocation(shader, "offsets");
    glm::mat4 trans(1.0f);
    float angle = 0.0f;
    //trans = camera(3.0f, glm::vec2(angle, angle));

    //trans = camera(10.0f, glm::vec2(1.0f, 0.0f));
    float const Translate = 10.0f;
    //glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 Projection = glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, 100.0f);
    //float near_plane = 1.0f, far_plane = 100.0f;
    //glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    
    
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, glm::pi<float>() * 0.25f, glm::vec3(1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, glm::pi<float>() * 0.25f, glm::vec3(0.0f, 1.0f, 0.0f));
    View = glm::rotate(View, glm::pi<float>() * 0.25f, glm::vec3(0.0f, 0.0f, 1.0f));
    
    //View = glm::lookAt(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    View = glm::lookAt(CAMERA_POS, CAMERA_FRONT, CAMERA_UP);
    MODEL = Model;
    VIEW = View;
    PROJECTION = Projection;
    trans = Projection * View * Model;
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
    
    


    /*-----------------------shadow texture-------------------------------*/
    //---------------------------------------------------------------------


    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.5f,  0.5f, -1.0f),
        glm::vec3(-0.5f, -0.5f, -1.0f),
        glm::vec3(-0.5f,  0.5f, -1.0f),
        glm::vec3(0.5f, -0.5f, -1.0f),

        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f),
    };

    glm::vec2 offsets[2] = { glm::vec2(0.5f,  0.5f), glm::vec2(-0.5f,  0.5f) };

    const size_t cubePositionsSize = sizeof(cubePositions) / sizeof(cubePositions[0]);

    //camera view----------------------
    const float radius = 1.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    //glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(view));
    // `T` is not limited by any constraints.
    
    //--------------------------------

    //instancing
    //glUniformMatrix2fv(vbOffsets, 1, GL_FALSE, glm::value_ptr(offsets[0]));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //transformation--------
        //angle += 0.01f;
        ////if (angle > 0.70f) angle = 0.60f;
        //std::cout << angle << std::endl;
        ////trans = glm::rotate(trans, angle, glm::vec3(0.5f, 1.0f, 0.5f));
        //trans = camera(2.0f, glm::vec2(angle, angle));
        //glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
        //-------------------

        // draw points 0-4 from the currently bound VAO with current in-use shader
        //glDrawArrays(GL_QUADS, 0, 3 * 4);

        //glDrawArraysInstanced(GL_QUADS, 0, 4, 2);
        
        //camera loot at---------------------
        //const float radius = 10.0f;
        //float camX = sin(glfwGetTime()) * radius;
        //float camZ = cos(glfwGetTime()) * radius;
        //glm::mat4 view;
        //View = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        ////View = glm::lookAt(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        //VIEW = View;
        //trans = PROJECTION * VIEW * MODEL;
        //glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
        //-------------------------------

        glDrawArrays(GL_QUADS, 0, 2 * 4);
        


        /* Poll for and process events */
        glfwPollEvents();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);


        /*slow down the loop*/
        util::handleloopCount(50, false);

    }


    //close all child thread
    _appThreadInfo.ALL_CHILD_THREADS_SOULD_CLOSE = 1;
    th1.join();
    glfwTerminate();
    return 0;

}

float getColor(float color)
{
    static float prevColor = 1.1f;
    if (color >= 0.0f && prevColor > color) {
        prevColor = color;
        std::cout << "color decreased" << std::endl;
        return decreaseColor(color);
    }
    else { //color is less that 0.0f
        prevColor = color;
        return increaseColor(color);
    }

}

float increaseColor(float color)
{
    color += 0.02f;
    return color;
}

float decreaseColor(float color)
{
    color -= 0.02f;
    return color;
}
