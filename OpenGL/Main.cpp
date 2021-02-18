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




glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model;
}





//const GLchar* const* string;

#define NAME_TO_STRING(X) #X


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

int main(void) {
	
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

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

    if (glewInit() != GLEW_OK)
        std::cout << "GLEW_ERROR" << std::endl;


    float vbo[] = {
        //pink triangle 1
         // positions           // colors
        -0.5f, -0.5f,  0.0f,    1.0f, 0.0f, 1.0f, 1.0f,  //left vertex
         0.5f, -0.5f,  0.0f,    1.0f, 0.0f, 1.0f, 1.0f,  //right vertex
         0.0f,  0.5f,  0.0f,    1.0f, 0.0f, 1.0f, 1.0f,  //top vertex

         //pink triangle 2
         // positions           // colors
        -0.5f, -0.5f,  0.05f,    1.0f, 0.0f, 1.0f, 1.0f,  //left vertex
         0.5f, -0.5f,  0.05f,    1.0f, 0.0f, 1.0f, 1.0f,  //right vertex
         0.0f,  0.5f,  0.05f,    1.0f, 0.0f, 1.0f, 1.0f,  //top vertex


         // positions           // colors
        -0.3f, -0.3f,  0.3f,    1.0f, 1.0f, 1.0f, 0.5f,
         0.3f, -0.3f,  0.3f,    1.0f, 1.0f, 1.0f, 0.5f,
         0.0f,  0.3f,  0.3f,    1.0f, 1.0f, 1.0f, 0.5f,

         

        //two triangles cover left side
        // positions           // colors
        -0.5f, -0.5f,  0.05f,   1.0f, 1.0f, 1.0f, 0.5f,
         0.0f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f,
         0.0f,  0.5f,  0.05f,   1.0f, 1.0f, 1.0f, 0.5f,

         // positions           // colors
        -0.5f, -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f,
         0.0f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f,
        -0.5f, -0.5f,  0.05f,   1.0f, 1.0f, 1.0f, 0.5f,
         
         //--------------------------

         //two triangles cover right side
        // positions           // colors
         0.5f, -0.5f, 0.05f,    1.0f, 1.0f, 1.0f, 0.5f, //right vertex z
         0.5f, -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f, //right vertex
         0.0f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f, //top vertex

         // positions           // colors
         0.5f, -0.5f, 0.05f,    1.0f, 1.0f, 1.0f, 0.5f, //right vertex z
         0.0f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f, //top vertex
         0.0f,  0.5f,  0.05f,   1.0f, 1.0f, 1.0f, 0.5f, //top vertex z
         //--------------------------


         //two triangles cover bottom side
        // positions           // colors
        -0.5f, -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f,  //left vertex
         0.5f, -0.5f,  0.0f,    1.0f, 1.0f, 1.0f, 0.5f,  //right vertex
         0.5f, -0.5f,  0.05f,   1.0f, 1.0f, 1.0f, 0.5f,  //right vertex z

         // positions           // colors
         0.5f, -0.5f,  0.05f,  1.0f, 1.0f, 1.0f, 0.5f,      //right vertex z
        -0.5f, -0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 0.5f,  //left vertex
        -0.5f, -0.5f,  0.05f,  1.0f, 1.0f, 1.0f, 0.5f,  //left vertex z
         //--------------------------


          // positions           // colors
         -0.3f, -0.3f, -0.3f,    1.0f, 1.0f, 1.0f, 0.5f,
          0.3f, -0.3f, -0.3f,    1.0f, 1.0f, 1.0f, 0.5f,
          0.0f,  0.3f, -0.3f,    1.0f, 1.0f, 1.0f, 0.5f

    };
    
    int bufferSize = (sizeof(vbo) / sizeof(vbo[0])) * sizeof(float);

    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    /*triangle 1 ----------------*/
    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, vbo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    GLuint points_colors_vbo;
    glGenBuffers(1, &points_colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, vbo, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    ///*triangle 2 ----------------*/
    //GLuint points_vbo1;
    //glGenBuffers(1, &points_vbo1);
    //glBindBuffer(GL_ARRAY_BUFFER, points_vbo1);
    //glBufferData(GL_ARRAY_BUFFER, bufferSize, vbo, GL_STATIC_DRAW);
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(2);


    //GLuint points_colors_vbo1;
    //glGenBuffers(1, &points_colors_vbo1);
    //glBindBuffer(GL_ARRAY_BUFFER, points_colors_vbo1);
    //glBufferData(GL_ARRAY_BUFFER, bufferSize, vbo, GL_STATIC_DRAW);
    //glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(3);



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
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("C:\\Users\\SALMAN-ALTAF\\Downloads\\img2.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    //---------------------------------------------------------------------------




    std::string vertexShader =
        "#version 400\r\n"
        
        "layout (location = 0) in vec3 vp;"
        "layout(location = 1) in vec4 vertexColor;"
        "out vec4 fragmentColor;"
        "uniform mat4 trans;"

        "void main() {"
        "   gl_Position = trans * vec4(vp, 1.0);"
        "   fragmentColor = vertexColor;"
        "}";
    /*gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;*/

    std::string fragmentShader =
        "#version 400\r\n"
        "out vec4 color;"
        "in vec4 fragmentColor;"
        "uniform vec4 newColor;"

        "void main() {"
        "   color = fragmentColor;"
        "}";

    //"  frag_colour = vec4(1, 0.0, 0.0, 1.0);"
    //"  color = vec4(fragmentColor, 1.0);"
    //"   color = vec4(fragmentColor, 1.0);"

    unsigned int shader = createShader(vertexShader, fragmentShader);
    
    glUseProgram(shader);

    GLint vbLocation = glGetUniformLocation(shader, "trans");
    GLint colorLocation = glGetUniformLocation(shader, "newColor");
    //glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans(1.0f);
    //trans = glm::rotate(trans, (float)45, glm::vec3(1.0f, 0.0f, 0.0f));

    

    float _color1 = 1.0f;
    float _color2 = 0.0f;
    glUniform4f(colorLocation, _color1, _color2, 0.0f, 1.0f);
    glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));

    float angle = 0.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        glEnable(GL_DEPTH_TEST);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        angle += 0.001f;
        if (angle > 0.10f) angle = 0.08f;
        std::cout << angle << std::endl;
        //transformation--------
        trans = glm::rotate(trans, angle, glm::vec3(0.5f, 1.0f, 0.5f));
        glUniformMatrix4fv(vbLocation, 1, GL_FALSE, glm::value_ptr(trans));
        //-------------------

        // draw points 0-4 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 10 * 3);


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
