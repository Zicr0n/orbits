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
    glm::vec3 initialVelocity = glm::vec3(0.0f);
    glm::vec3 startPosition = glm::vec3(0.0f);

    CelestialBody(Model* model, float _mass, float _radius, glm::vec3 initialVel, glm::vec3 initialPos) : model(model) {
        initialVelocity = initialVel;
        this->startPosition = initialPos;
        this->position = initialPos;
        mass = _mass;
        radius = _radius;
        Awake();
    }

    void Init(){
        Awake();
    }
    
    void UpdateVelocity(const std::vector<CelestialBody>& allBodies, float timeStep, size_t selfIndex){
        for (size_t i = 0; i < allBodies.size(); i++)
        {
            if (i == selfIndex) continue;

            const CelestialBody& other = allBodies[i];
            glm::vec3 difference = other.position - this->position;
            
            float sqrDist = glm::dot(difference, difference);
            if (sqrDist < 0.0001f) continue; // avoid division by zero on collision
            
            glm::vec3 forceDir = glm::normalize(difference);
            glm::vec3 acceleration = forceDir * gravitationalConstant * other.mass / sqrDist;
            this->currentVelocity += acceleration * timeStep;
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

    void Reset(){
        this->position = startPosition;
        currentVelocity = glm::vec3(0.0f);
    }

    void SetStartPosition(glm::vec3 newPos){
        this->startPosition = newPos;
        this->position = newPos;
    }

    void SetInitialVelocity(glm::vec3 newVel){
        this->initialVelocity = newVel;
    }

private:
    void Awake(){
        currentVelocity = initialVelocity;
    }

    glm::vec3 currentVelocity = glm::vec3(0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    Model* model;
};

#endif