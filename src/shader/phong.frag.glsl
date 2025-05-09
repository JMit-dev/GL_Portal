#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 lightPos = vec3(3.0,3.0,3.0);
uniform vec3 lightColor = vec3(1.0);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);

    // spec
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 color = (0.1 + diff)*vec3(0.8,0.5,0.3) + spec*lightColor;
    FragColor = vec4(color,1.0);
}
