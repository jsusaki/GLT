//https://youtu.be/aWdySZ0BtJs

#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screen_texture;
uniform vec2 screen_resolution;

// Scanline
uniform float scanline_amplitude;
uniform float scanline_frequency;
uniform float scanline_offset;

// Phosphor dot
uniform float phosphor_dot_scale;     // How many dot cells per unit; e.g. 640.0 for high-res CRT dots
uniform float phosphor_dot_softness;  // Controls softness of dot edges, e.g., 0.15

// Barrel Distortion 
uniform float primary_curvature;
uniform float secondary_curvature;

// Vignette
uniform float vignette_radius;   // Radius where the vignette effect starts
uniform float vignette_softness; // Controls the softness of the vignette transition

// Gaussian Blur
uniform float blur_radius;  // Controls how far to sample (e.g., 1.0/textureWidth)
uniform float blend_factor; // How much of the blur to mix in (0.0 to 1.0)

// Gaussian kernel
const float kernel[9] = float[](
    0.0625, 0.125, 0.0625,
    0.125,  0.25,  0.125,
    0.0625, 0.125, 0.0625
);

// Color Correction
uniform float gamma;
uniform float contrast;
uniform float saturation;
uniform float brightness;
uniform float color_correction;

vec3 GaussianBlur(vec2 uv) 
{
    vec3 sum = vec3(0.0);
    int index = 0;
    for (int y = -1; y <= 1; y++) 
    {
        for (int x = -1; x <= 1; x++) 
        {
            vec2 offset = vec2(float(x), float(y)) * blur_radius;
            sum += texture(screen_texture, uv + offset).rgb * kernel[index];
            index++;
        }
    }
    return sum;
}

vec2 BarrelDistort(vec2 uv, float k1, float k2) 
{
    uv = uv * 2.0 - 1.0; // Convert uv coordinate from [0,1] to [-1,1]
    float r2 = dot(uv, uv); // Radial distance from center
    float factor = 1.0 + k1 * r2 + k2 * r2 * r2; // radial distortion
    uv *= factor;
    return uv * 0.5 + 0.5; // Convert uv coordinate back to [0,1]
}

vec3 PostEffects(in vec3 rgb) 
{
    rgb = pow(rgb, vec3(gamma));
    rgb = mix(vec3(0.5), mix(vec3(dot(vec3(0.2125, 0.7154, 0.0721), rgb * brightness)), rgb * brightness, saturation), contrast);
    return rgb;
}

// Bloom
uniform float bloom_intensity;    // How strong the bloom is added
uniform float bloom_threshold;    // Luminance threshold for bloom extraction
uniform float bloom_blend_factor; // Controls blending (blurred vs. raw bright pass)

void main() 
{
    // Extract uv coordinates
    vec2 uv = TexCoords;

    // Barrel Distortion
    uv = BarrelDistort(uv, primary_curvature, secondary_curvature);

    // Color bleeding
    vec3 original_color = texture(screen_texture, uv).rgb; // Extract color from screen texture
    vec3 blurred_color  = GaussianBlur(uv);
    vec3 gauss_mix      = mix(original_color, blurred_color, blend_factor); // Blend the blurred color with the original color.
    
    vec3 final_color = gauss_mix;

    if (uv.x <= 0.0 || uv.x >= 1.0 || uv.y <= 0.0 || uv.y >= 1.0)
        final_color = vec3(0.0);

    // Scanline
    float scanline = scanline_amplitude * sin(uv.y * scanline_frequency) + scanline_offset;
    final_color *= scanline;

    /*
    // Phosphor dot
    vec2 dotUV = TexCoords * phosphor_dot_scale;
    vec2 fuv = fract(dotUV) - 0.5;
    float dotMask = 1.0 - smoothstep(0.4, 0.4 - phosphor_dot_softness, length(fuv));
    final_color *= dotMask;
    */

    // Vignette
    float dist     = distance(uv, vec2(0.5, 0.5));
    float vignette = smoothstep(vignette_radius, vignette_radius - vignette_softness, dist);
    final_color    *= vignette;
   
    // Bloom
    vec3 scene_color = texture(screen_texture, uv).rgb;
    // Calculate luminance using Rec.709 coefficients.
    float luminance = dot(scene_color, vec3(0.2126, 0.7152, 0.0722));
    // Extract bright parts (only pixels above bloom_threshold contribute)
    vec3 brightPass = scene_color * step(bloom_threshold, luminance);
    // Apply a basic blur to the bright parts (again, using the GaussianBlur function)
    vec3 blurredBright = GaussianBlur(uv);
    // Blend the raw bright pass with its blurred version to get a smoother bloom.
    vec3 bloom_effect = mix(brightPass, blurredBright, bloom_blend_factor);
    // Add the bloom effect on top of the CRT effect.
    final_color += bloom_effect * bloom_intensity;

    // Apply color correction
    final_color = mix(final_color, PostEffects(final_color), color_correction);

    FragColor = vec4(clamp(final_color, vec3(0.0), vec3(1.0)), 1.0);
}
