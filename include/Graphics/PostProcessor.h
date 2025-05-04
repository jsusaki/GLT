#pragma once

#include <glad/glad.h>
#include <vector>

#include "Core/Common.h"
#include "Graphics/Shader.h"

struct quad_vert
{
    vf2 p;
    vf2 uv;
};

// 2D Post Processing Pipeline
class PostProcessor
{
public:
    PostProcessor(s32 width, s32 height);
    ~PostProcessor();

public:
    // Call before scene rendering
    void Begin();

    // Call after scene rendering 
    void End();

    void Render();

private:
    u32 FBO;                 // Framebuffer object for screen
    u32 framebuffer_texture; // The texture attached to the framebuffer

    // Quad
    u32 VAO; // Quad VAO
    u32 VBO; // Quad VBO
    std::vector<quad_vert> verts; // Quad Vertex data 
};