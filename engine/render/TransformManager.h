//
// Created by l1nuvv on 14.06.2025.
//
#pragma once

#ifndef TRANSFORMMANAGER_H
#define TRANSFORMMANAGER_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransformManager
{
public:
    static glm::mat4 CreateModelMatrix(const glm::vec3& position = glm::vec3(0.0f),
                                       const glm::vec3& rotation = glm::vec3(0.0f),
                                       const glm::vec3& scale = glm::vec3(1.0f));

    static glm::mat4 CreateViewMatrix(const glm::vec3& cameraPos,
                                      const glm::vec3& target,
                                      const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

    static glm::mat4 CreateProjectionMatrix(float fov, GLfloat aspectRatio, float nearPlane = 0.1f,
                                            float farPlane = 100.0f);

    static glm::mat4 CreateRotatingModel(float time, float speed = 1.0f,
                                         const glm::vec3& axis = glm::vec3(0.0f, 0.0f, 1.0f));

    static glm::mat4 CreateOrbitingCamera(float time, float radius = 3.0f, float height = 0.0f);
};


#endif //TRANSFORMMANAGER_H
