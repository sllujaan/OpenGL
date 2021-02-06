#include"pch.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtx/string_cast.hpp>


#include"Util.h"



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


int main(void) {
	

    //glm::transpose(myMatrix);

    glm::mat4 m4(1.0f); // construct identity matrix
    m4[0];     // column zero

    m4[0].x;          // same as m4[ 0 ][ 0 ]
    m4[0].y = 2.0f;   // same as m4[ 0 ][ 1 ]
    m4[0].z = 3.0f;   // same as m4[ 0 ][ 2 ]
    m4[0].w = 4.0f;   // same as m4[ 0 ][ 3 ]

    m4[1].x;          // same as m4[ 1 ][ 0 ]
    m4[1].y = 2.0f;   // same as m4[ 1 ][ 1 ]
    m4[1].z = 3.0f;   // same as m4[ 1 ][ 2 ]
    m4[1].w = 4.0f;   // same as m4[ 1 ][ 3 ]

    std::cout << glm::to_string(m4) << std::endl;

    glm::vec3 v3(1.0f, 1.0f, 1.0f);

    std::cout << glm::to_string(v3) << std::endl;

    glm::translate(m4, v3);



    std::cout << glm::to_string(glm::translate(m4, v3)) << std::endl;


























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

    



    //glfwSetErrorCallback(error_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
     


    if (glewInit() != GLEW_OK)
        std::cout << "GLEW_ERROR" << std::endl;

    /*float points[] = {
       0.0f,  0.5f,  0.0f,
       0.5f, -0.5f,  0.0f,
      -0.5f, -0.5f,  0.0f
    };*/

    float points[] = {
       -0.5f, -0.5f,
        0.5f, -0.5f,
        0.0f,  0.5f
    };

    float newPoints[] = {
       -0.2f, -0.2f,
        0.2f, -0.2f,
        0.0f,  0.2f
    };

    float points_colors[] = {
       1.0f,  0.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  0.0f,  1.0f
    };


    
    /*points 1 ----------------*/
    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(0);



    /*colors -------------------*/
    GLuint points_colors_vbo;
    glGenBuffers(1, &points_colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);



    std::string vertexShader =
        "#version 400\r\n"
        "layout(location = 0) in vec2 vp;"
        "layout(location = 1) in vec3 vertexColor;"
        "out vec3 fragmentColor;"
        "void main() {"
        "   gl_Position = vec4(vp, 0.0, 1.0);"
        "   fragmentColor = vertexColor;"
        "}";
    /*gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;*/

    std::string fragmentShader =
        "#version 400\r\n"
        "out vec4 color;"
        "in vec3 fragmentColor;"
        "uniform vec4 ourColor;"
        "void main() {"
        "  color = ourColor;"
        "}";

    //"  frag_colour = vec4(1, 0.0, 0.0, 1.0);"
    //"  color = vec4(fragmentColor, 1.0);"

    unsigned int shader = createShader(vertexShader, fragmentShader);
    
    glUseProgram(shader);



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        // update the uniform color
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shader, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


        // draw points 0-4 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 3);


        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        

        /*slow down the loop*/
        util::handleloopCount(200, false);

    }

    glfwTerminate();
    return 0;



	/*std::cout << glfwInit() << std::endl;

	std::cin.get();*/
}