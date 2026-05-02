#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 LightPos;

uniform sampler2D texture1;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    vec3 color = texture(texture1, TexCoord).rgb;
    vec3 ambient = 0.1 * color;
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.3) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
