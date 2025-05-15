#pragma once
#include <iostream>
#include <string>

#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Core/Common.h"

class Texture
{
public:
	Texture(s32 width, s32 height, s32 channels = 4, bool filtered = false, bool clamped = true, bool mipmap = false);
	Texture(const std::string& filepath, bool flip_vertically = true, bool filtered = true, bool clamped = false);
	~Texture();

public:
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	GLuint GetID() const;
	const std::string& GetPath() const;
	s32 GetWidth() const;
	s32 GetHeight() const;

	bool LoadFromFile(const std::string& filepath, bool flip_vertically = true);
	void Create(int width, int height, const unsigned char* data, int channels, bool filtered = true, bool clamped = false, bool mipmap = true);

private:
	GLuint m_id = 0;
	std::string m_path;
	s32 m_width = 0;
	s32 m_height = 0;
	s32 m_channels = 0;
};