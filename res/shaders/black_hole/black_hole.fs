#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screen_texture;
uniform vec2 resolution;

uniform vec2  bh_center;
uniform float schwarzschild_radius;
uniform float lensing_strength;
uniform float einstein_radius;
uniform float accretion_thickness;
uniform float accretion_intensity;
uniform float chroma_offset;
uniform float blur_strength;

uniform float photon_ring_intensity;
uniform float photon_ring_thickness;

void main() 
{
    vec2 uv      = TexCoords;
    float aspect = resolution.x / resolution.y;
    vec2 dir     = uv - bh_center;
         dir     = vec2(dir.x * aspect, dir.y);
    float r      = length(dir);

    float r_s    = schwarzschild_radius * 0.5;

    vec4 color = texture(screen_texture, uv);

    // Gravitational lensing (Distortion)
    if (r < schwarzschild_radius * 4.0) 
    {
        float bend  = lensing_strength / max(r, 1e-4);
        bend       *= smoothstep(schwarzschild_radius * 4.0, r_s, r);

        // Deflection
        float ang = bend * 0.5;
        float cs  = cos(ang);
        float sn  = sin(ang);
        dir = vec2(cs * dir.x - sn * dir.y,
                   sn * dir.x + cs * dir.y);

        float alpha = (einstein_radius * einstein_radius) / max(r, 1e-4);
        float r_src = max(r - alpha, 0.0);

        uv = bh_center + normalize(dir) * r_src;

        // Chromatic aberration
        vec2 offset     = normalize(dir) * chroma_offset;
        vec4 col_r      = texture(screen_texture, uv + offset);
        vec4 col_g      = texture(screen_texture, uv);
        vec4 col_b      = texture(screen_texture, uv - offset);
        vec4 lens_color = vec4(col_r.r, col_g.g, col_b.b, 1.0);

        // Radial blur
        int samples = 6;
        vec4 blur_accum = vec4(0.0);
        for (int i = 0; i < samples; i++) 
        {
            float t = float(i) / float(samples - 1);
            blur_accum += texture(screen_texture, uv + dir * blur_strength * t);
        }
        blur_accum /= float(samples);

        // Accretion disk
        float disk      = smoothstep(accretion_thickness, 0.0, abs(r - schwarzschild_radius * 4.0));
        vec3 disk_color = vec3(1.0, 0.7, 0.3) * disk * accretion_intensity;

        color.rgb = mix(lens_color.rgb, blur_accum.rgb, 0.4) + disk_color;
        color.a   = 1.0;
    }

    // Photon sphere
    float photon_sphere = 1.5 * schwarzschild_radius;
    float ring = smoothstep(photon_ring_thickness, 0.0, abs(r - photon_sphere));
    color.rgb += vec3(1.0) * ring * photon_ring_intensity;

    // Event Horizon
    float mask = step(schwarzschild_radius, r);
    color      = mix(vec4(0.0, 0.0, 0.0, 1.0), color, mask);

    FragColor = color;
}
