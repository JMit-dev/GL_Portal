#version 330 core   
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

uniform mat4 model, view, proj;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 worldPos = model * vec4(aPos,1.0);

    FragPos       = worldPos.xyz;
    Normal        = mat3(transpose(inverse(model))) * aNormal;

    gl_Position        = proj * view * worldPos;
}
