#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec4 Color;
out vec3 FragPos;
out vec3 Normal;
out float GlowIntensity;

uniform float t;
uniform vec3  audio;
uniform mat4  proj_view;
uniform vec4  col;

vec3 elegant(vec3 pos)
{
    // Audio-reactive height with smooth flowing bands
    float height = 0.0;
    // Bass: Smooth traveling waves
    float bassWave = sin((pos.z - t * 2.0) * 0.8) * sin(pos.x * 0.3) * audio.x * 5.0 * 3;
    height += bassWave;
    // Mid: Diagonal wave pattern
    float midWave = sin((pos.x + pos.z) * 0.9 - t * 3.5) * cos((pos.x - pos.z) * 0.5 + t * 2.0) * audio.y * 2.5 * 3;
    height += midWave;
    // Treble: Fast surface ripples
    float trebleWave = sin(pos.x * 3.0 - t * 5.0) *  cos(pos.z * 3.0 + t * 4.0) * audio.z * 1.2 * 3;
    height += trebleWave;
    // Smooth baseline undulation
    height += sin(pos.x * 0.2 + t * 0.5) * cos(pos.z * 0.2 - t * 0.3) * 0.75;
    pos.y += height;
    return pos;
}

vec3 elegant2(vec3 pos)
{
    float height = 0.0;
    
    // Layer 1: Sub-bass - Very large, slow undulations (foundation)
    float subBass = sin(pos.x * 0.15 + t * 0.8) * cos(pos.z * 0.15 - t * 0.6) * audio.x * 12.0;
    height += subBass;
    
    // Layer 2: Bass - Smooth traveling waves
    float bassWave = sin((pos.z - t * 2.0) * 0.8) * sin(pos.x * 0.3) * audio.x * 6.0;
    height += bassWave;
    
    // Layer 3: Low-mid - Rolling hills effect
    float lowMid = sin(pos.x * 0.5 + pos.z * 0.3 - t * 2.5) * cos(pos.x * 0.3 - pos.z * 0.5 + t * 1.8) * audio.y * 5.0;
    height += lowMid;
    
    // Layer 4: Mid - Diagonal wave pattern
    float midWave = sin((pos.x + pos.z) * 0.9 - t * 3.5) * cos((pos.x - pos.z) * 0.5 + t * 2.0) * audio.y * 4.0;
    height += midWave;
    
    // Layer 5: High-mid - Circular ripples from center
    float dist = length(pos.xz * 0.2);
    float highMid = sin(dist * 5.0 - t * 4.0 + audio.z * 3.0) * audio.z * 2.0;
    height += highMid;
    
    // Layer 6: Treble - Fast surface ripples
    float trebleWave = sin(pos.x * 3.0 - t * 5.0) * cos(pos.z * 3.0 + t * 4.0) * audio.z * 2.0;
    height += trebleWave;
    
    // Layer 7: High frequency detail - Fine texture
    float detail = sin(pos.x * 5.0 + t * 6.0) * sin(pos.z * 4.5 - t * 5.5) * audio.z * 1.0;
    height += detail;
    
    // Layer 8: Interference pattern - Cross waves
    float interference = sin(pos.x * 1.2 - t * 3.2) * sin(pos.z * 1.3 + t * 2.8) * (audio.x + audio.y) * 1.2;
    height += interference;
    
    // Layer 9: Smooth baseline undulation (always present)
    float baseline = sin(pos.x * 0.2 + t * 0.5) * cos(pos.z * 0.2 - t * 0.3) * 1.2;
    height += baseline;
    
    // Layer 10: Audio-reactive turbulence
    float turbulence = sin(pos.x * 0.8 + sin(t * 1.5) * 2.0) * cos(pos.z * 0.7 + cos(t * 1.3) * 2.0) * (audio.x + audio.y + audio.z) * 0.6;
    height += turbulence;
    
    pos.y += height;
    return pos;
}

vec3 elegant3(vec3 pos)
{
    float height = 0.0;
    
    // First Layer
    
    // Bass foundation - Large sweeping waves
    float bass1 = sin((pos.z - t * 2.0) * 0.8) * sin(pos.x * 0.3) * audio.x * 5.0;
    float bass2 = sin((pos.x - t * 1.5) * 0.6) * cos(pos.z * 0.4) * audio.x * 4.0;
    height += bass1 + bass2 * 0.6;
    
    // Mid frequencies - Multiple diagonal patterns
    float mid1 = sin((pos.x + pos.z) * 0.9 - t * 3.5) * 
                 cos((pos.x - pos.z) * 0.5 + t * 2.0) * audio.y * 2.5;
    float mid2 = sin((pos.x * 0.7 + pos.z * 1.2) - t * 4.0) * audio.y * 2.0;
    float mid3 = cos((pos.x * 1.1 - pos.z * 0.8) + t * 3.0) * audio.y * 1.5;
    height += mid1 + mid2 * 0.7 + mid3 * 0.5;
    
    // Treble - Fast ripples and detail
    float treble1 = sin(pos.x * 3.0 - t * 5.0) * cos(pos.z * 3.0 + t * 4.0) * audio.z * 1.2;
    float treble2 = sin(pos.x * 4.5 + t * 6.0) * sin(pos.z * 4.2 - t * 5.5) * audio.z * 0.8;
    float treble3 = cos(pos.x * 6.0 - t * 7.0) * cos(pos.z * 5.8 + t * 6.5) * audio.z * 0.4;
    height += treble1 + treble2 * 0.6 + treble3 * 0.3;
    
    // Second layer
    
    // Radial waves from center
    float radialDist = length(pos.xz);
    float radial = sin(radialDist * 2.0 - t * 3.0 + audio.x * 2.0) * 
                   (audio.x + audio.y) * 1.5;
    height += radial;
    
    // Rotating spiral pattern
    float angle = atan(pos.z, pos.x);
    float spiral = sin(radialDist * 3.0 - angle * 2.0 - t * 2.5) * 
                   (audio.y + audio.z) * 1.0;
    height += spiral * 0.5;
    
    // Interference pattern
    float wave1 = sin(pos.x * 1.5 - t * 3.0);
    float wave2 = sin(pos.z * 1.3 + t * 2.5);
    float interference = wave1 * wave2 * (audio.x + audio.y + audio.z) * 0.8;
    height += interference;
    
    // Breathing/pulsing effect
    float pulse = sin(t * 1.5) * cos(t * 1.2) * (audio.x + audio.y + audio.z) * 0.5;
    height += pulse;
    
    // Baseline smooth undulation (always present)
    float baseline = sin(pos.x * 0.2 + t * 0.5) * cos(pos.z * 0.2 - t * 0.3) * 0.75;
    height += baseline;
    
    // Noise/texture layer
    float noise = sin(pos.x * 7.0 + t * 8.0) * cos(pos.z * 6.5 - t * 7.5) * 0.2;
    height += noise * (audio.z * 0.5);
    
    pos.y += height * 2.0;
    return pos;
}

void main()
{
    vec3 pos = vec3(aPos.x, aPos.z, aPos.y);
    
    // Audio-reactive height
    pos = elegant2(pos);
    float height = pos.y;  

    Color = col;

    FragPos = pos;

    gl_Position = proj_view * vec4(pos, 1.0);
}