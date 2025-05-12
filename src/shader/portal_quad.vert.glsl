#version 330 core
layout(location=0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uPortalVP;   // = P_dst * V_dst

out vec4 vProjPos;

void main(){
    vec4 worldPos = uModel * vec4(aPos,1);
    gl_Position   = uProj * uView * worldPos;
    vProjPos      = uPortalVP * worldPos;
}
