#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform float acc_time;
uniform vec2 resolution;

void pulse()
{
    // Centered UV coordinates
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoords, center);

    // Radius of the dot
    float radius = 50.0;

    // Pulsing intensity using sine wave
    float pulse = 0.5 + 0.5 * sin(acc_time * 2.0); // oscillates between 0 and 1

    // Smooth fade based on distance from center
    float alpha = smoothstep(radius, radius * 0.25, dist);

    // Final color: red with pulsing brightness and fading alpha
    vec3 color = vec3(pulse, 0.0, 0.0); // red only
    FragColor = vec4(color, 1.0 - alpha); // fade out from center
}

void main()
{
    pulse();
}

