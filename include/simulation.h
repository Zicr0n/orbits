#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <deque>
#include "celestialBody.h"
#include "universe.h"
#include "camera.h"
#include "shader.h"

class Simulation {

public:
    std::vector<CelestialBody> bodies;

    Simulation(Camera* cam, Shader* shaderProgram, unsigned int& w, unsigned int& h)
        : shader(shaderProgram), camera(cam), scrWidth(w), scrHeight(h) {}

    void StartSimulation(){
        trails.clear();
        trails.resize(bodies.size());
        for (auto& body : bodies)
            body.Init();
    }

    void EndSimulation(){
        trails.clear();
        for (size_t i = 0; i < bodies.size(); i++) {
            bodies[i].Reset();
        }
    }

    void AddBody(CelestialBody body){
        bodies.push_back(body);
        trails.push_back({});
    }

    void RemoveBody(size_t index) {
        bodies.erase(bodies.begin() + index);
        trails.erase(trails.begin() + index);
    }

    void Update(){
        float dt = timeStep;

        static float accumulator = 0.0f;

        float frameTime = timeStep * timeScale;
        accumulator += frameTime;

        const float fixedDt = timeStep;
        
        while (accumulator >= fixedDt){

        // Gör en lista med accelerationer för varje himlakropp
        std::vector<glm::vec3> accelerations(bodies.size(), glm::vec3(0.0f));

         // Gå igenom varje himlakropp
        for (size_t i = 0; i < bodies.size(); i++) {
            // För varje himlakropp, gå igenom resten
            for (size_t j = 0; j < bodies.size(); j++) {
                if (i == j) continue; // Om det är samma, skippa

                // Räkna skillnaden i position, riktningen
                glm::vec3 diff = bodies[j].position - bodies[i].position;

                // Räkna ut avståndet och kalkulera inte om avståndet är för litet
                float r = glm::length(diff);
                if (r < 1e-3f) continue;

                // accelerationStorlek = G * M / r^2, där M = andra himlakroppen , G ör gravitationskonstanten
                float accelerationMagnitude = gravitationalConstant * bodies[j].mass / (r * r);

                // Lägg till accelerationen för kroppen med indexet "i"
                accelerations[i] += glm::normalize(diff) * accelerationMagnitude;
            }
        }

        // Loopa igenom alla himlakroppar
        for (size_t i = 0; i < bodies.size(); i++) {
            // Addera accelerationen multiplicerad med tidsskillnaden till hastigheten
            bodies[i].currentVelocity += accelerations[i] * fixedDt;

            // Addera hastigheten multiplicerad med tidsskillnaden till positionen
            bodies[i].position += bodies[i].currentVelocity * fixedDt; 
        }

        accumulator -= fixedDt;
        }

        for (size_t i = 0; i < bodies.size(); i++)
        {
            trails[i].push_back(bodies[i].position * simulationScale);

            if (trails[i].size() > maxTrailLength)
                trails[i].pop_front();
        }
    }

    void Render(){
        shader->use();

        glm::mat4 projection = glm::perspective(
            glm::radians(camera->Zoom),
            (float)scrWidth / (float)scrHeight,
            0.1f, 100000.0f
        );
        glm::mat4 view = camera->GetViewMatrix();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        // Draw trails
        glm::mat4 identity = glm::mat4(1.0f);
        shader->setMat4("model", identity);

        for (size_t i = 0; i < bodies.size(); i++)
            renderTrail(i);

        // Draw bodies
        for (auto& body : bodies)
            body.Render(shader);
    }

    void FocusAll(Camera& cam) {
        if (bodies.empty()) return;

        glm::vec3 center(0.0f);
        for (const auto& body : bodies)
            center += body.position;
        center /= (float)bodies.size();

        float maxDist = 0.0f;
        for (const auto& body : bodies)
            maxDist = glm::max(maxDist, glm::length(body.position - center));

        cam.Position = center + glm::vec3(0.0f, 0.0f, maxDist * 10.0f);
        cam.LookAt(center);
    }

    size_t maxTrailLength = 500;

    std::vector<glm::vec3> ComputeAccelerations()
    {
        std::vector<glm::vec3> acc(bodies.size(), glm::vec3(0.0f));

        for (size_t i = 0; i < bodies.size(); i++)
        {
            for (size_t j = 0; j < bodies.size(); j++)
            {
                if (i == j) continue;

                glm::vec3 diff = bodies[j].position - bodies[i].position;

                float r2 = glm::dot(diff, diff);
                if (r2 < 1e-6f) continue;

                float invR = 1.0f / sqrt(r2);
                float invR3 = invR * invR * invR;

                acc[i] += gravitationalConstant *
                        bodies[j].mass *
                        diff * invR3;
            }
        }

        return acc;
    }

private:
    std::vector<std::deque<glm::vec3>> trails;

    unsigned int trailVAO = 0;
    unsigned int trailVBO = 0;

    Shader* shader;
    Camera* camera;
    unsigned int& scrWidth;
    unsigned int& scrHeight;

    void initTrailBuffers(){
        glGenVertexArrays(1, &trailVAO);
        glGenBuffers(1, &trailVBO);

        glBindVertexArray(trailVAO);
        glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
        glBufferData(GL_ARRAY_BUFFER, maxTrailLength * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void renderTrail(size_t index)
    {
        if (index >= trails.size())
            return;

        const auto& trail = trails[index];

        if (trail.size() < 2)
            return;

        // Lazy init
        if (trailVAO == 0)
            initTrailBuffers();

        // Copy deque to contiguous vector
        std::vector<glm::vec3> points(trail.begin(), trail.end());

        glBindVertexArray(trailVAO);
        glBindBuffer(GL_ARRAY_BUFFER, trailVBO);

        // Resize buffer if needed
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);

        // Fade color toward transparent at the tail
        glm::vec4 bodyColor = bodies[index].color;
        shader->setVec4("color", glm::vec4(bodyColor.r, bodyColor.g, bodyColor.b, 0.3f));

        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(points.size()));

        glBindVertexArray(0);

        // Restore body color for next draw
        shader->setVec4("color", bodyColor);
    }
};

#endif