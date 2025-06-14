#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 color;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
    ourColor = color;
}
