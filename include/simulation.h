#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "celestialBody.h"
#include "universe.h"
#include "camera.h"
#include "shader.h"

class Simulation {

public:
    Simulation(std::vector<CelestialBody>& allBodies, Camera* cam, Shader* shaderProgram)
        : bodies(allBodies), shader(shaderProgram), camera(cam) {

    }

    void Update(){
        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].UpdateVelocity(bodies, timeStep);
        }

        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].UpdatePosition(timeStep);

        }
    }

    void Render(){
        shader->use();

        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)1280 / (float)720, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        for (size_t i = 0; i < bodies.size(); i++)
        {
            bodies[i].Render(shader);
        }
    }

private:
    std::vector<CelestialBody> bodies;
    float fixedDeltaTime = timeStep;

    Shader* shader;
    Camera* camera;
};

#endif