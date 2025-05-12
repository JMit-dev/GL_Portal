#version 330 core
in vec2 vUV; // <-- flat UVs from vertex shader

uniform sampler2D portalTex;
out vec4 FragColor;

void main()
{
    FragColor = texture(portalTex, vUV);
}
