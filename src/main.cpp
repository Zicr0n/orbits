#include <iostream>
#include <string>
#include <format>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "simulation.h"
#include "celestialBody.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 100.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  

class Triangle {
    public : 
        unsigned int VAO;
        unsigned int VBO;

        Triangle(){
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            this->VAO = VAO;
        };
        void Draw(){
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
};

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Orbits", nullptr, nullptr);

    if (!window)
    {
        std::cout << "Failed to create window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);
    stbi_set_flip_vertically_on_load(true);

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Triangle triangle;

    Shader defaultShader("shaders/shader.vs", "shaders/shader.fs");

    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    Model ourModel("models/backpack/backpack.obj");
    
    // Simulation
    Model planetModel("models/backpack/backpack.obj");

    std::vector<CelestialBody> bodies;

    CelestialBody planet(
        &planetModel,
        20.0f,
        1.0f,
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f)
    );

    CelestialBody moon(
        &planetModel,
        1.0f,
        0.5f,
        glm::vec3(28.28f, 0.0f, 0.0f),
        glm::vec3(0.0f, 20.0f, 0.0f)
    );

    bodies.push_back(planet);
    bodies.push_back(moon);

    Simulation mySimulation(bodies, &camera, &ourShader);

    bool drawSimulation = true;
    float sliderVal = 0.0f;
    float color[4] = {0.0f, 1.0f, 1.0f, 1.0f};

    bool isSimulationRunning = false;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        defaultShader.use();
        triangle.Draw();

        if (isSimulationRunning){
            mySimulation.Update();
        }

        if(drawSimulation){
            mySimulation.Render();
        }

        // // don't forget to enable shader before setting uniforms
        // ourShader.use();

        // // view/projection transformations
        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // glm::mat4 view = camera.GetViewMatrix();
        // ourShader.setMat4("projection", projection);
        // ourShader.setMat4("view", view);

        // // render the loaded model
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        // ourShader.setMat4("model", model);
        // ourModel.Draw(ourShader);

        // Simulation

        ImGui::Begin("Hello world!");
        ImGui::Text("Hello there!");
        ImGui::Checkbox("Draw?", &drawSimulation);

        if (!isSimulationRunning){
            if(ImGui::Button("Start Simulation!")){
                std::cout << "Pressed Start!" << '\n';
                isSimulationRunning = true;
                mySimulation.StartSimulation();
            }

            for (size_t i = 0; i < mySimulation.bodies.size(); i++)
            {
                ImGui::PushID(i);
                std::string label = "Celestial Body " + std::to_string(i);

                if (ImGui::CollapsingHeader(label.c_str())){
                    ImGui::Text("Celestial body");
    
                    CelestialBody& body = mySimulation.bodies[i];
                    
                    glm::vec3 pos = body.startPosition;
                    if (ImGui::SliderFloat3("Initial Position", glm::value_ptr(pos), -100.0f, 100.0f))
                    {
                        body.SetStartPosition(pos); // only called when slider is changed
                    }
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::PopID();
            }

        } else{
            if(ImGui::Button("End Simulation!")){
                std::cout << "Pressed End!" << '\n';
                isSimulationRunning = false;
                mySimulation.EndSimulation();
            }

        }

        ImGui::SliderFloat("Slider", &sliderVal, 0.0f, 1e3f);
        ImGui::ColorEdit4("Color", color);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);

    glfwTerminate();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

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

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}