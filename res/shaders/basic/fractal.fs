#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform vec2  screen_size;
uniform float zoom;
uniform int   max_iter;

uniform vec2  c;
uniform bool  show_julia;

uniform vec2 center_offset;

// Classic
vec4 map_to_color(float t, vec3 col) 
{
    float b = col.x * (1.0 - t) * t * t * t;
    float g = col.y * (1.0 - t) * (1.0 - t) * t * t;
    float r = col.z * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;

    return vec4(r, g, b, 1.0);
}

// from Mattz
vec3 magma(float t) 
{ 
    const vec3 c0 = vec3(-0.002136485053939582, -0.000749655052795221, -0.005386127855323933);
    const vec3 c1 = vec3(0.2516605407371642,     0.6775232436837668,    2.494026599312351);
    const vec3 c2 = vec3(8.353717279216625,     -3.577719514958484,     0.3144679030132573);
    const vec3 c3 = vec3(-27.66873308576866,    14.26473078096533,    -13.64921318813922);
    const vec3 c4 = vec3(52.17613981234068,    -27.94360607168351,     12.94416944238394);
    const vec3 c5 = vec3(-50.76852536473588,    29.04658282127291,      4.23415299384598);
    const vec3 c6 = vec3(18.65570506591883,    -11.48977351997711,    -5.601961508734096);
    t *= 2.0; if (t >= 1.0) { t = 2.0 - t; }
    return c0 + t * (c1 + t * ( c2 + t * (c3 + t * (c4 + t *(c5 + t * c6)))));
}

float fractal(vec2 z, vec2 c, int max_iter)
{
    int iter;
    for (iter = 0; iter < max_iter; iter++) 
    {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (2.0 * z.x * z.y)       + c.y;

        if ((x * x + y * y) > 32.0)
            break;

        z = vec2(x, y);
    }
    
    // Lower color banding
    float smooth_iter = float(iter);
    if (iter < max_iter) 
    {
        float abs_z = dot(z, z);
        abs_z = max(abs_z, 1e-8);
        smooth_iter = float(iter) + 1.0 - log2(log2(sqrt(abs_z)));
    }
    float t = smooth_iter / float(max_iter);


    // Normal color banding
    //float t = float(iter) / float(max_iter);

    return t;
}

float julia(vec2 texcoords, float aspect, float zoom, vec2 c, vec2 center_offset, int max_iter)
{
    vec2 z;
    z.x = (texcoords.x - 0.5) * zoom * aspect * 2.0 + center_offset.x;
    z.y = (texcoords.y - 0.5) * zoom          * 2.0 + center_offset.y;         

    return fractal(z, c, max_iter);
}

float mandelbrot(vec2 texcoords, float aspect, float zoom, vec2 center_offset, int max_iter)
{
    vec2 c, z;
    c.x = (texcoords.x - 0.5) * zoom * aspect * 2.0 + center_offset.x - 0.5;
    c.y = (texcoords.y - 0.5) * zoom          * 2.0 + center_offset.y;         
    z = vec2(0.0);

    return fractal(z, c, max_iter);
}

void main()
{
    float aspect = screen_size.x / screen_size.y;
  
    float t;
    if (show_julia)    
        t = julia(TexCoords, aspect, zoom, c, center_offset, max_iter);
    else
        t = mandelbrot(TexCoords, aspect, zoom, center_offset, max_iter);
    
    //FragColor = vec4(vec3(t), 1.0);
    //FragColor = map_to_color(t, vec3(9.0, 15.0, 8.5));
    FragColor = vec4(magma(t), 1.0);
}

