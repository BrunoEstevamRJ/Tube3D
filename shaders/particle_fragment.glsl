#version 330 core

in float vLife;

out vec4 FragColor;

void main() {
    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    float radius = dot(uv, uv);
    if (radius > 1.0) {
        discard;
    }

    float core = pow(max(0.0, 1.0 - radius), 4.0);
    float glow = pow(max(0.0, 1.0 - radius), 1.5);
    vec3 color = mix(vec3(1.0, 0.55, 0.08), vec3(1.0, 0.97, 0.82), core);
    float alpha = glow * mix(0.15, 0.95, vLife);

    FragColor = vec4(color * (glow + core * 1.8), alpha);
}
