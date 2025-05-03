// https://www.shadertoy.com/view/XdyGzR
#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screen_texture;
uniform vec2 screen_resolution;

uniform float gamma;
uniform float contrast;
uniform float saturation;
uniform float brightness;

uniform float lightIntensity;
uniform float blurAmount;
uniform float curvature;
uniform float scanlineIntensity;
uniform float colorCorrection;

vec3 postEffects(in vec3 rgb) 
{
    rgb = pow(rgb, vec3(gamma));
    rgb = mix(vec3(0.5), mix(vec3(dot(vec3(0.2125, 0.7154, 0.0721), rgb * brightness)), rgb * brightness, saturation), contrast);
    return rgb;
}

// Gaussian Blur
vec3 gaussianBlur(in vec2 uv) 
{
    float b = blurAmount / (screen_resolution.x / screen_resolution.y);

    uv += 0.5;
    vec3 col = texture(screen_texture, vec2(uv.x - b/screen_resolution.x, uv.y - b/screen_resolution.y)).rgb * 0.077847;
    col += texture(screen_texture, vec2(uv.x - b/screen_resolution.x, uv.y)).rgb * 0.123317;
    col += texture(screen_texture, vec2(uv.x - b/screen_resolution.x, uv.y + b/screen_resolution.y)).rgb * 0.077847;

    col += texture(screen_texture, vec2(uv.x, uv.y - b/screen_resolution.y)).rgb * 0.123317;
    col += texture(screen_texture, vec2(uv.x, uv.y)).rgb * 0.195346;
    col += texture(screen_texture, vec2(uv.x, uv.y + b/screen_resolution.y)).rgb * 0.123317;

    col += texture(screen_texture, vec2(uv.x + b/screen_resolution.x, uv.y - b/screen_resolution.y)).rgb * 0.077847;
    col += texture(screen_texture, vec2(uv.x + b/screen_resolution.x, uv.y)).rgb * 0.123317;
    col += texture(screen_texture, vec2(uv.x + b/screen_resolution.x, uv.y + b/screen_resolution.y)).rgb * 0.077847;

    return col;
}

void main() 
{
    vec2 st = (TexCoords - vec2(0.5)); 

    // Curvature effect
    float d = length(st * 0.5 * st * 0.5);
    vec2 uv = mix(st, st * d + st * 0.935, curvature);

    // Blur effect
    vec3 color = mix(texture(screen_texture, uv + 0.5).rgb, gaussianBlur(uv), blurAmount);

    // Light effect
    float l = 1.0 - min(1.0, d * lightIntensity);
    color *= l;

    // Scanlines
    float y = mix(uv.y, st.y, 1.0 - curvature);
    float s = 1.0 - smoothstep(320.0, 1440.0, screen_resolution.y) + 1.0;
    float j = cos(y * screen_resolution.y * s) * scanlineIntensity;
    color = mix(color, color - color * j, scanlineIntensity);

    // Screen border mask (vignette))
    float mask = max(0.0, 1.0 - 2.0 * max(abs(uv.x), abs(uv.y)));
    mask = min(mask * 200.0, 1.0);
    color *= mix(1.0, mask, curvature);

    // Color correction
    color = mix(color, postEffects(color), colorCorrection);

    FragColor = vec4(clamp(color, vec3(0.0), vec3(1.0)), 1.0);
}
