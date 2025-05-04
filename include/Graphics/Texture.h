#pragma once
#include <iostream>
#include <string>

#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Common.h"

class Texture
{
public:
	Texture();
	Texture(const std::string& filepath, bool flip_vertically = true, bool filtered = true, bool clamped = false);
	~Texture();

public:
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	GLuint GetID() const;
	const std::string& GetPath() const;
	int GetWidth() const;
	int GetHeight() const;

	bool LoadFromFile(const std::string& filepath, bool flip_vertically = true);
	void Create(int width, int height, const unsigned char* data, int channels, bool filtered = true, bool clamped = false, bool mipmap = true);

private:
	GLuint m_id = 0;
	std::string m_path;
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
};