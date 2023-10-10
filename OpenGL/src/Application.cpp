#include"Renderer.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "failed to init GLFW" << std::endl;
        return -1;
    }

    //Enable OPENGL Core profile, Enableing the Core profile removes the default vertexArray and sets the OPENGL version to the one specified.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "window terminated" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // set fps to monitor refresh rate
    glfwSwapInterval(1);

    //init glad
    int version = gladLoadGL(glfwGetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    {
        // indicies to create a square
        unsigned int indices[]{
            0,1,2,
            2,3,0
        };

        //vertex positions of a square
        float positions[]{
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };

        //docs.gl for documentation
        //start creating OpenGL elements needed for rendering
        VertexArray vao;
        VertexBuffer vbo(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        vao.AddBuffer(vbo, layout);

        IndexBuffer ibo(indices, 6);

        Shader shader("res/shaders/basic.shader");
        shader.Bind();

        shader.SetUniform4f("u_Color", 0.2f, 0.5f, 0.8f, 1.0f);

        //unbind so we can rebind correct elements at draw call
        vao.Unbind();
        shader.Unbind();
        vbo.Unbind();
        ibo.Unbind();

        Renderer renderer;

        //change red color value based on offset;
        float a = 0.0f;
        float offset = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            //TODO: convert to use materials
            //bind the correct elements before next draw call
            shader.Bind();
            //set the uniform values before next draw call
            shader.SetUniform4f("u_Color", a, 0.5f, 0.8f, 1.0f);
           
            renderer.Draw(vao,ibo,shader);

            if (a > 1.0f)
            {
                offset = -0.05f;
            }
            else if (a < 0.0f)
            {
                offset = 0.05f;
            }
            a += offset;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}