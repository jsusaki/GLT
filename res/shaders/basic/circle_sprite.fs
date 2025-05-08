#version 330 core

in vec4 vColor;
in float vSpeed;
out vec4 FragColor;

void main() 
{
    vec2 d = gl_PointCoord - vec2(0.5);
    if (length(d) > 0.5)
        discard;

    float brightness = mix(0.5, 1.0, vSpeed);
    FragColor = vec4(vColor.rgb, vColor.a * brightness* brightness);
}
