// portal.vert
#version 330 core
layout(location=0) in vec3 aPos;

uniform mat4 uModel;      // portal’s model → world
uniform mat4 uPortalVP;   // Pdst * Vdst, passed in C++
uniform mat4 uView, uProj;

out vec2 vUV;

void main() {
    // draw the portal frame
    vec4 worldPos = uModel * vec4(aPos,1.0);
    gl_Position   = uProj * uView * worldPos;

    // project into the portal‐camera’s clip space
    vec4 clipPos = uPortalVP * vec4(aPos,1.0);

    // perspective‐divide → NDC → UV in [0,1]
    vec2 uv = clipPos.xy/clipPos.w * 0.5 + 0.5;

    // clamp to [0,1] so you never sample outside the texture
    vUV = clamp(uv, 0.0, 1.0);
}
