//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#ifndef TRIANGLEAPP_H
#define TRIANGLEAPP_H

#include "../engine/core/Application.h"

class TriangleApp: public Application
{
public:
    TriangleApp() : Application(800, 600, "TriangleApp") {}
    void Initialize() override;
    void Render() override;
    void Shutdown() override;
    //virtual void OnWindowResize(int width, int height) override;

private:
    GLuint m_shaderProgram = 0;
    GLuint m_VAO           = 0;
    GLuint m_VBO           = 0;
};
#endif // TRIANGLEAPP_H
