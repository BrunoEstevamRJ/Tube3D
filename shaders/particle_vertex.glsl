#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aLife;
layout(location = 2) in float aSize;

uniform mat4 view;
uniform mat4 projection;

out float vLife;

void main() {
    vec4 viewPos = view * vec4(aPos, 1.0);
    gl_Position = projection * viewPos;

    float depthScale = 1.0 / max(0.35, -viewPos.z);
    gl_PointSize = aSize * (24.0 * depthScale) * mix(0.45, 1.0, aLife);
    vLife = aLife;
}
