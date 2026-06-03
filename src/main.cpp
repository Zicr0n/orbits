#include <iostream>
#include <string>

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

unsigned int SCR_WIDTH  = 1280;
unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseEnabled = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Orbits", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.5f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Shader ourShader("shaders/shader.vs", "shaders/color.fs");
    Model planetModel("models/celestialBody/celestialbody.obj");

    Simulation mySimulation(&camera, &ourShader, SCR_WIDTH, SCR_HEIGHT);

    bool drawSimulation = true;
    bool isSimulationRunning = false;
    bool isSimulationPaused = false;

    float _mass   = 1.0f;
    float _radius = 1.0f;
    float _color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    glm::vec3 _vel = glm::vec3(0.0f);
    glm::vec3 _pos = glm::vec3(0.0f);

    float m = 1.0f;

    CelestialBody body1(
        &planetModel,
        m,
        0.1f,
        glm::vec3( 0.4662036850f,  0.4323657300f, 0.0f),
        glm::vec3(-0.9700043600f,  0.2430875300f, 0.0f),
        glm::vec4(1, 0, 0, 1)
    );

    CelestialBody body2(
        &planetModel,
        m,
        0.1f,
        glm::vec3( 0.4662036850f,  0.4323657300f, 0.0f),
        glm::vec3( 0.9700043600f, -0.2430875300f, 0.0f),
        glm::vec4(1, 1, 0, 1)
    );

    CelestialBody body3(
        &planetModel,
        m,
        0.1f,
        glm::vec3(-0.9324073700f, -0.8647314600f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec4(0, 1, 1, 1)
    );

    mySimulation.AddBody(body1);
    mySimulation.AddBody(body2);
    mySimulation.AddBody(body3);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (isSimulationRunning && !isSimulationPaused)
            mySimulation.Update();

        if (drawSimulation)
            mySimulation.Render();

        ImGui::Begin("Simulation");
        ImGui::Checkbox("Draw", &drawSimulation);
        ImGui::Spacing();
        

        if (ImGui::CollapsingHeader("Universe"))
        {
            ImGui::SliderFloat("Gravitational Constant", &gravitationalConstant, 0.0f, 10000.0f);
            if (ImGui::Button("Reset Gravitational Constant"))
                resetGravConstant();
            ImGui::SliderFloat("Simulation Scale", &simulationScale, 0.001f, 3.0f);
            ImGui::SliderFloat("Time Scale", &timeScale, 0.01f, 1000.0f);
        }

        ImGui::Spacing();

        if (!isSimulationRunning)
        {
            if (ImGui::Button("Start Simulation"))
            {
                isSimulationRunning = true;
                mySimulation.StartSimulation();
            }

            ImGui::Spacing();

            if (ImGui::CollapsingHeader("Create Celestial Body"))
            {
                ImGui::SliderFloat3("Position",  glm::value_ptr(_pos), -10000.0f, 10000.0f);
                ImGui::SliderFloat3("Velocity",  glm::value_ptr(_vel), -20000.0f, 20000.0f);
                ImGui::SliderFloat("Mass",   &_mass,   0.0f, 100000.0f);
                ImGui::SliderFloat("Radius", &_radius, 0.0f, 5000.0f);
                ImGui::ColorEdit4("Color", _color);

                if (ImGui::Button("Create Body"))
                {
                    mySimulation.AddBody(CelestialBody(
                        &planetModel, _mass, _radius, _vel, _pos,
                        glm::vec4(_color[0], _color[1], _color[2], _color[3])
                    ));
                }
            }

            ImGui::Spacing();

            for (size_t i = 0; i < mySimulation.bodies.size(); i++)
            {
                ImGui::PushID(static_cast<int>(i));
                std::string label = "Body " + std::to_string(i);

                if (ImGui::CollapsingHeader(label.c_str()))
                {
                    CelestialBody& body = mySimulation.bodies[i];

                    glm::vec3 pos = body.startPosition;
                    if (ImGui::SliderFloat3("Start Position", glm::value_ptr(pos), -10000.0f, 10000.0f))
                        body.SetStartPosition(pos);

                    glm::vec3 vel = body.initialVelocity;
                    if (ImGui::SliderFloat3("Start Velocity", glm::value_ptr(vel), -20000.0f, 20000.0f))
                        body.SetInitialVelocity(vel);

                    float mass = body.mass;
                    if (ImGui::SliderFloat("Mass", &mass, 0.0f, 100000.0f))
                        body.mass = mass;

                    float radius = body.radius;
                    if (ImGui::SliderFloat("Radius", &radius, 0.0f, 5000.0f))
                        body.radius = radius;

                    float col[4] = { body.color.r, body.color.g, body.color.b, body.color.a };
                    if (ImGui::ColorEdit4("Color", col))
                        body.color = glm::vec4(col[0], col[1], col[2], col[3]);

                    ImGui::Spacing();
                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
                    bool removed = ImGui::Button("Remove");
                    ImGui::PopStyleColor(3);

                    if (removed)
                    {
                        mySimulation.RemoveBody(i);
                        ImGui::PopID();
                        break;
                    }
                }

                ImGui::Spacing();
                ImGui::PopID();
            }
        }
        else
        {
            if (ImGui::Button("Focus All"))
                mySimulation.FocusAll(camera);

            if (!isSimulationPaused)
            {
                if (ImGui::Button("Pause"))
                    isSimulationPaused = true;
            }
            else
            {
                if (ImGui::Button("Continue"))
                    isSimulationPaused = false;
            }

            if (ImGui::Button("End Simulation"))
            {
                isSimulationRunning = false;
                isSimulationPaused  = false;
                mySimulation.EndSimulation();
            }
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle mouse look with Tab
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        mouseEnabled = !mouseEnabled;
        glfwSetInputMode(window, GLFW_CURSOR,
            mouseEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }

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
    if (!mouseEnabled) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset =  xpos - lastX;
    float yoffset =  lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH  = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}