#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform float acc_time;
uniform vec2 resolution;

// Distance to a set of "neuron" points
float field(vec2 uv, vec2 pos, float t) {
    float d = length(uv - pos);
    return sin(10.0 * d - t * 5.0) / (10.0 * d + 0.2);
}

void main() {
    vec2 uv = TexCoords;
    float t = acc_time;

    float intensity = 0.0;

    // Simulated neuron positions
    vec2 neurons[5] = vec2[](
        vec2(0.3, 0.3),
        vec2(0.7, 0.3),
        vec2(0.5, 0.5),
        vec2(0.3, 0.7),
        vec2(0.7, 0.7)
    );

    for (int i = 0; i < 5; ++i) {
        intensity += field(uv, neurons[i], t);
    }

    // Glow
    vec3 col = vec3(0.2, 0.8, 1.0) * intensity;

    FragColor = vec4(col, 1.0);
}

