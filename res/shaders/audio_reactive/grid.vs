#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec4 Color;
out vec3 FragPos;
out vec3 Normal;

uniform float t;
uniform vec3  audio;
uniform mat4  proj_view;
uniform vec4  col;

vec3 compute_height_field(vec3 pos)
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



void main()
{
    // Audio-reactive height
    vec3 pos = vec3(aPos.x, aPos.z, aPos.y);
    pos      = compute_height_field(pos);

    Color    = col;
    FragPos  = pos;
    gl_Position = proj_view * vec4(pos, 1.0);
}