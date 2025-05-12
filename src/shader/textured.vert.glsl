#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

uniform mat4 model, view, proj;

out vec2 vUV;

void main()
{
    vec4 worldPos      = model * vec4(aPos, 1.0);
    gl_Position        = proj * view * worldPos;

    vUV = aUV;
}
