#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "celestialBody.h"
#include "universe.h"
#include "camera.h"
#include "shader.h"

class Simulation {

public:
    std::vector<CelestialBody> bodies;

    Simulation(std::vector<CelestialBody>& allBodies, Camera* cam, Shader* shaderProgram)
        : bodies(allBodies), shader(shaderProgram), camera(cam) {

    }

    void StartSimulation(){
        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].Init();
        }
    }

    void EndSimulation(){
         for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].Reset();
        }
    }

    void AddBody(CelestialBody body){
        bodies.push_back(body);
    }

    void Update(){
        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].UpdateVelocity(bodies, timeStep,i);
        }

        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].UpdatePosition(timeStep);

        }
    }

    void Render(){
        shader->use();

        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)1280 / (float)720, 0.1f, 100000.0f);
        glm::mat4 view = camera->GetViewMatrix();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].Render(shader);
        }
    }

private:
    float fixedDeltaTime = timeStep;

    Shader* shader;
    Camera* camera;
};

#endif