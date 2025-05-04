#include "Texture.h"

#include <iostream>

#include "stb_image.h"

Texture::Texture() {}

Texture::Texture(const std::string& filepath, bool flip_vertically, bool filtered, bool clamped)
{
    LoadFromFile(filepath, flip_vertically);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

bool Texture::LoadFromFile(const std::string& filepath, bool flip_vertically)
{
    stbi_set_flip_vertically_on_load(flip_vertically);
    unsigned char* data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data)
    {
        std::cerr << "ERROR: Failed to load texture: " << filepath << "\n";
        return false;
    }

    m_path = filepath;
    Create(m_width, m_height, data, m_channels, true, false, true);
    stbi_image_free(data);
    return true;
}

void Texture::Create(int width, int height, const unsigned char* data, int channels, bool filtered, bool clamped, bool mipmap)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    GLenum format = GL_RGB;
    switch (channels)
    {
    case 1: format = GL_RED;  break;
    case 2: format = GL_RG;   break;
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtered ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtered ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     clamped ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     clamped ? GL_CLAMP_TO_EDGE : GL_REPEAT);

    if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::GetID() const { return m_id; }
const std::string& Texture::GetPath() const { return m_path; }
int Texture::GetWidth() const { return m_width; }
int Texture::GetHeight() const { return m_height; }
