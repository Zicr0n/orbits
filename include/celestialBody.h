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
    glm::vec3 currentVelocity = glm::vec3(0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    
    glm::vec3 initialVelocity = glm::vec3(0.0f);
    glm::vec3 startPosition = glm::vec3(0.0f);

    glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    CelestialBody(Model* model, float _mass, float _radius, glm::vec3 initialVel, glm::vec3 initialPos, glm::vec4 initColor ) : model(model) {
        initialVelocity = initialVel;
        this->startPosition = initialPos;
        this->position = initialPos;
        mass = _mass;
        radius = _radius;
        color = initColor;

        Awake();
    }

    void Init(){
        Awake();
    }

    void Render(Shader* shader){
        glm::mat4 model_mat = glm::mat4(1.0f);
        model_mat = glm::translate(model_mat, this->position * simulationScale);
        model_mat = glm::scale(model_mat, glm::vec3(radius * simulationScale));  // scale radius too

        shader->setMat4("model", model_mat);
        shader->setVec4("color", color);

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


    Model* model;
};

#endif