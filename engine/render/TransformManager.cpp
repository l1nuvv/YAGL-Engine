//
// Created by l1nuvv on 14.06.2025.
//

#include "TransformManager.h"
#include "GLFW/glfw3.h"


glm::mat4 TransformManager::CreateModelMatrix(const glm::vec3 &position,
                                              const glm::vec3 &rotation,
                                              const glm::vec3 &scale)
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    return model;
}

glm::mat4 TransformManager::CreateViewMatrix(const glm::vec3 &cameraPos,
                                             const glm::vec3 &target,
                                             const glm::vec3  up)
{
    return glm::lookAt(cameraPos, target, up);
}

glm::mat4 TransformManager::CreateProjectionMatrix(float fov, GLfloat     aspectRatio,
                                                   float nearPlane, float farPlane)
{
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 TransformManager::CreateRotatingModel(float time, float speed, const glm::vec3 &axis)
{
    glm::mat4 model = glm::mat4(1.0f);
    return glm::rotate(model, time * glm::radians(50.0f * speed), axis);
}

glm::mat4 TransformManager::CreateOrbitingCamera(float time, float radius, float height)
{
    glm::vec3 cameraPos = glm::vec3(
            cos(time) * radius,
            height,
            sin(time) * radius
            );

    return glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
