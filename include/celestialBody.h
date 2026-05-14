#pragma once
#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include "universe.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

class CelestialBody 
{
public:
    float mass;
    float radius;
    glm::vec3 position = glm::vec3(0.0);
    glm::vec3 initialVelocity = glm::vec3(0.0);

    CelestialBody(Model* model, float _mass, float _radius, glm::vec3 initialVel, glm::vec3 initialPos) : model(model) {
        initialVelocity = initialVel;
        position = initialPos;
        mass = _mass;
        radius = _radius;
        Awake();
    }
    
    void UpdateVelocity(const std::vector<CelestialBody>& allBodies, float timeStep){

        for (size_t i = 0; i < allBodies.size(); i++)
        {
            const CelestialBody& other = allBodies.at(i);

            if (&other != this) {
                glm::vec3 difference = other.position - this->position;
                
                float sqrDist = glm::dot(difference, difference);
                glm::vec3 forceDir = glm::normalize(difference);
                glm::vec3 force = forceDir * gravitationalConstant * mass * other.mass / sqrDist;
                glm::vec3 acceleration = force / mass;
                this->currentVelocity += acceleration * timeStep;

                std::cout<<glm::to_string(this->currentVelocity)<<std::endl;
            }
        }
    }

    void UpdatePosition(float timeStep) {
        this->position += this->currentVelocity * timeStep;
    }

    void Render(Shader* shader){
        glm::mat4 model_mat = glm::mat4(1.0f);
        model_mat = glm::translate(model_mat, this->position);

        shader->setMat4("model", model_mat);

        model->Draw(*shader);
    }

private:
    void Awake(){
        currentVelocity = initialVelocity;
    }

    glm::vec3 currentVelocity = glm::vec3(0.0);
    Model* model;
};

#endif