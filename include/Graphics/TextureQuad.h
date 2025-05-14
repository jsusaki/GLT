#pragma once

#include <glad/glad.h>
#include <vector>

#include "Core/Common.h"

struct TextureQuad
{
    struct vertex
    {
        vf3 p;
        vf3 c;
        vf2 uv;
    };

    TextureQuad()
    {
        vertices = {
            // positions             // colors              // texcoords
            {{ 1.0f, -1.0f, 0.0f },  { 1.0f, 0.0f, 0.0f },  { 1.0f, 0.0f }}, // bottom right
            {{-1.0f, -1.0f, 0.0f },  { 0.0f, 1.0f, 0.0f },  { 0.0f, 0.0f }}, // bottom left
            {{ 1.0f,  1.0f, 0.0f },  { 0.0f, 0.0f, 1.0f },  { 1.0f, 1.0f }}, // top right
            {{-1.0f,  1.0f, 0.0f },  { 1.0f, 1.0f, 0.0f },  { 0.0f, 1.0f }}  // top left
        };

        indices = {
            0, 1, 2, // First triangle
            1, 2, 3  // Second triangle
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        // Bind VAO
        glBindVertexArray(vao);

        // Bind and upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

        // Bind and upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, p));
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, c));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);
    }

    ~TextureQuad()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void Draw()
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    std::vector<vertex> vertices;
    std::vector<u32> indices;
    u32 vbo;
    u32 vao;
    u32 ebo;
};