#version 460 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

// Uniform переменные для градиента
uniform float time;
uniform vec3 colorStart;
uniform vec3 colorEnd;

// Uniform переменные для текстуры
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    // Градиент
    float t = (sin(time) + 1.0f) / 2.0f;
    vec3 gradientColor = mix(colorStart, colorEnd, t);

    // Привязка текстуры
    FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2f);
}
