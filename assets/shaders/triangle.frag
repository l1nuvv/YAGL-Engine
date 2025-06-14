#version 460 core

out vec4 FragColor;

uniform float time;
uniform vec3 colorStart;
uniform vec3 colorEnd;

void main()
{
    float t = (sin(time) + 1.0f) / 2.0f;
    vec3 gradientColor = mix(colorStart, colorEnd, t);
    FragColor = vec4(gradientColor, 1.0f);
}
