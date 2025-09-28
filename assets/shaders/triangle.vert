#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    //gl_Position = transform * vec4(aPosition, 1.0);
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    ourColor = aColor;
    TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
}
