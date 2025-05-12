#version 330 core
in vec4 vProjPos;
uniform sampler2D portalTex;

out vec4 FragColor;
void main(){
    // perspective‐divide → NDC in [-1,1]
    vec2 ndc = vProjPos.xy / vProjPos.w;
    // map to [0,1]
    vec2 uv  = ndc * 0.5 + 0.5;
    if(uv.x<0.0||uv.x>1.0||uv.y<0.0||uv.y>1.0) discard;
    FragColor = texture(portalTex, uv);
}
