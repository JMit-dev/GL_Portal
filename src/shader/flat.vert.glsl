#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec4 uClipPlane;         

void main()
{
    vec4 worldPos      = model * vec4(aPos, 1.0);
    gl_Position        = proj * view * worldPos;

    gl_ClipDistance[0] = dot(worldPos, uClipPlane);   
}
