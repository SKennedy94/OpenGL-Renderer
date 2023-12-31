#include"Renderer.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "[OpenGL Version]: " << glGetString(GL_VERSION) << std::endl;

    {
        // indicies to create a square
        unsigned int indices[]{
            0,1,2,
            2,3,0
        };

        //vertex positions of a square
    // Vertici positions // UV Coords //
        float positions[]{
            100.0f, 100.0f, 0.0f, 0.0f,   //BL
             200.0f, 100.0f, 1.0f, 0.0f,   //BR
             200.0f,  200.0f, 1.0f, 1.0f,   //TR
            100.0f,  200.0f, 0.0f, 1.0f    //TL
        };
    
        //need to enable blending and create a blend func with parameters for blending (Textures)
        //Math is (srcR * srcA) + (destR * (1 - srcA)) = blended R value do this for all color channels
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //docs.gl for documentation
        //start creating OpenGL elements needed for rendering
        VertexArray vao;
        VertexBuffer vbo(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vao.AddBuffer(vbo, layout);

        IndexBuffer ibo(indices, 6);
        /*Model matrix: defines position, rotation and scale of the vertices of the model in the world.
          View matrix: defines position and orientation of the "camera".
          Projection matrix: Maps what the "camera" sees to NDC, taking care of aspect ratio and perspective.
          All are combined to determine normalized device cordinates */
        glm::mat4 proj = glm::ortho(0.0f,640.0f,0.0f,480.0f,-1.0f,1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100,0,0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200,200,0));

        glm::mat4 mvp = proj * view * model;

        Shader shader("res/shaders/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", glm::vec4(0.2f, 0.5f, 0.8f, 1.0f)); 

        Texture texture("res/textures/brick.jpg");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //unbind so we can rebind correct elements at draw call
        vao.Unbind();
        shader.Unbind();
        vbo.Unbind();
        ibo.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();

        glm::vec3 translation(200, 200, 0);

        //change red color value based on offset;
        float a = 0.0f;
        float offset = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
            glm::mat4 mvp = proj * view * model;

            //TODO: convert to use materials
            //bind the correct elements before next draw call
            shader.Bind();
            //set the uniform values before next draw call
            shader.SetUniform4f("u_Color", glm::vec4(a, 0.5f, 0.8f, 1.0f));
            shader.SetUniformMat4f("u_MVP", mvp);

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

            ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 640.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}