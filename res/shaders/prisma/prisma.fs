// http://www.pouet.net/prod.php?which=57245
#version 330 core

out vec4 FragColor;

uniform float t;
uniform vec2  r;

void main()
{
    vec3  c = vec3(0.0);
    float l;
    float z = t;

    vec2 fragCoord = gl_FragCoord.xy;

    for (int i = 0; i < 3; i++)
    {
        vec2 uv, p = fragCoord / r;
        uv = p;

        // --- space ---
        p -= 0.5;
        p.x *= r.x / r.y;

        // bass: geometry scaling
        p *= 1.0 + bass * 0.25;

        // --- time / phase ---
        z += 0.07 + mid * 0.5;

        l = max(length(p), 1e-4);

        // --- warp ---
        uv += p / l * (sin(z) + 1.0) * abs(sin(l * 9.0 - z - z));

        // treble: detail intensity
        c[i] = (0.01 + tre * 0.02) / max(length(mod(uv, 1.0) - 0.5), 1e-4);
    }

    FragColor = vec4(c / l, 1.0);
}