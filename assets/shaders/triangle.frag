#version 460 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform float time;
uniform vec3 colorStart;
uniform vec3 colorEnd;
uniform sampler2D ourTexture;

void main()
{
    float t = (sin(time) + 1.0f) / 2.0f;
    vec3 gradientColor = mix(colorStart, colorEnd, t);
    FragColor = texture(ourTexture, TexCoord) * vec4(gradientColor, 1.0f);
}
