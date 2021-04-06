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




glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model;
}





void translateShader(transDirection dir) {
    static float translateX = 0.0f;
    static float translateY = 0.0f;

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
    float angle = 0.0f;
    //trans = camera(translate, glm::vec2(angle, angle));
    trans = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, 0.0f));
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
}


void rotateScene(rotateDir dir) {
    static float rotateX = 0.0f;
    static float rotateY = 0.0f;

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
    View = camera(1.0f, glm::vec2(rotateX, rotateY));
    //View = glm::rotate(View, rotateX, glm::vec3(0.0f, 0.0f, 0.0f));
    //View = glm::rotate(View, rotateY, glm::vec3(0.0f, 0.0f, 0.0f));

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

    static float translate = 0.0f;
    GLint vbLocation = glGetUniformLocation(SHADER_GLOBAL, "trans");

    if (zoom == Zoom::In) {
        translate -= 0.1f;
    }
    else if (zoom == Zoom::Out) {
        translate += 0.1f;
    }
    //pass trans camera to the shader
    glm::mat4 trans(1.0f);
    float angle = 0.0f;
    //trans = camera(translate, glm::vec2(angle, angle));
    trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, -2.0));

    
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
}


bool firstMouse = false;
float lastX = 400, lastY = 300;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

        float sensitivity = 0.01f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        std::cout << xoffset << ", " << yoffset << std::endl;
        

        glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(xoffset, yoffset, 0.0f));
        glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(View));
        
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed a key" << std::endl;
        //translateShader();
        //translateShaderCamera();
        translateShader(transDirection::up);
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed a key" << std::endl;
        //translateShader();
        //translateShaderCamera();
        translateShader(transDirection::down);
    }

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed a key" << std::endl;
        //translateShader();
        //translateShaderCamera();
        translateShader(transDirection::left);
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed a key" << std::endl;
        //translateShader();
        //translateShaderCamera();
        translateShader(transDirection::right);
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed key up" << std::endl;
        rotateScene(rotateDir::up);
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed key up" << std::endl;
        rotateScene(rotateDir::down);
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed key up" << std::endl;
        rotateScene(rotateDir::left);
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        std::cout << "your presssed key up" << std::endl;
        rotateScene(rotateDir::right);
    }

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

    if (glewInit() != GLEW_OK)
        std::cout << "GLEW_ERROR" << std::endl;


    float vbo[] = {
        // positions            //color                //normals
       -0.2f, -0.2f,  0.0f,      1.0f, 1.0f, 0.0f,     -0.2f, -0.2f, -1.0f,   //left bottom
       -0.2f,  0.2f,  0.0f,      1.0f, 1.0f, 0.0f,     -0.2f,  0.2f, -1.0f,   //left top
        0.2f,  0.2f,  0.0f,      1.0f, 1.0f, 0.0f,      0.2f,  0.2f, -1.0f,   //right top
        0.2f, -0.2f,  0.0f,      1.0f, 1.0f, 0.0f,      0.2f, -0.2f, -1.0f,   //right bottom
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
        ""

        "void main() {"
        "   vec3 offsets[2] = {vec3(0.5f, 0.5f, 0.5f), vec3(-0.5f,  0.5f, 0.5f)};"
        "   vec3 offset = offsets[gl_InstanceID];"
        "   gl_Position = trans * vec4(vp + offset, 1.0);"
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
        "   vec3 lightPos = vec3(0.0, 0.0, -10.0);"
        "   vec3 viewPos = vec3(0.0, 0.0, 0.0);"
        "   float ambientStrength = 0.1;"
        "   vec3 ambient = ambientStrength * lightColor;"
        "   vec3 norm = normalize(Normal);"
        "   vec3 lightDir = normalize(lightPos - FragPos);"

        "   float diff = max(dot(norm, lightDir), 0.0);"
        "   vec3 diffuse = diff * lightColor;"

        "   float specularStrength = 0.5;"
        "   vec3 viewDir = normalize(viewPos - FragPos);"
        "   vec3 reflectDir = reflect(-lightDir, norm);"
        
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
        "   vec3 specular = specularStrength * spec * lightColor;"

        "   vec3 result = (ambient + diffuse + specular) * objectColor;"

        "   color = vec4(result, 1.0f);"
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
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
    



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


    //--------------------------------

    //instancing
    //glUniformMatrix2fv(vbOffsets, 1, GL_FALSE, glm::value_ptr(offsets[0]));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
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
        glDrawArrays(GL_QUADS, 0, 3 * 4);

        glDrawArraysInstanced(GL_QUADS, 0, 4, 2);

        //glDrawArrays(GL_QUADS, 0, 1 * 4);
        


        /* Poll for and process events */
        glfwPollEvents();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);


        /*slow down the loop*/
        util::handleloopCount(50, false);

    }



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
