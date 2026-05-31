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

    void RemoveBody(size_t index) {
        bodies.erase(bodies.begin() + index);
    }

    void Update(){
        std::vector<glm::vec3> accelerations(bodies.size(), glm::vec3(0.0f));
        
        // Calculate accelerations
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = 0; j < bodies.size(); j++) {
                if (i == j) continue;

                glm::vec3 diff = bodies[j].position - bodies[i].position;
                float r = glm::length(diff);
                if (r < 0.0001f) continue;

                float force = gravitationalConstant * bodies[j].mass / (r * r);
                accelerations[i] += glm::normalize(diff) * force;
            }
        }

        // apply accelerations
        for (size_t i = 0; i < bodies.size(); i++) {
            bodies[i].currentVelocity += accelerations[i] * timeStep * timeScale;
            bodies[i].position += bodies[i].currentVelocity * timeStep * timeScale;
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

    void FocusAll(Camera& camera) {
        if (bodies.empty()) return;

        glm::vec3 center(0.0f);
        for (const auto& body : bodies)
            center += body.position;

        center /= bodies.size();

        float maxDist = 0.0f;
        for (const auto& body : bodies)
            maxDist = glm::max(maxDist, glm::length(body.position - center));

        camera.Position = center + glm::vec3(0.0f, 0.0f, maxDist * 10.0f);
        camera.LookAt(center);
    }

private:
    float fixedDeltaTime = timeStep;

    Shader* shader;
    Camera* camera;
};

#endif