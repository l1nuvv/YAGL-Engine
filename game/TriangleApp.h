//
// Created by l1nuvv on 08.06.2025.
//

#pragma once

#include "../engine/core/Application.h"

class TriangleApp: public Application
{
public:
    TriangleApp() : Application(800, 600, "TriangleApp") {}
    virtual void Initialize() override;
    virtual void Render() override;
    virtual void Shutdown() override;
    //virtual void OnWindowResize(int width, int height) override;

private:
    GLuint m_shaderProgram = 0;
    GLuint m_VAO           = 0;
    GLuint m_VBO           = 0;
};