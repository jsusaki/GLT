#pragma once

#include "Graphics/Texture.h"
#include "Graphics/TextureQuad.h"

struct Sprite
{
	std::unique_ptr<TextureQuad> m_quad;
	std::unique_ptr<Texture> m_texture;

	std::vector<u8> m_pixels;
	s32 m_width;
	s32 m_height;

	Sprite(const std::string& filepath)
	{
		m_texture = std::make_unique<Texture>(filepath);
		m_quad = std::make_unique<TextureQuad>();
	}

	Sprite(s32 width, s32 height)
	{
		m_width = width;
		m_height = height;
		m_texture = std::make_unique<Texture>(m_width, m_height);
		m_quad = std::make_unique<TextureQuad>();
		m_pixels.resize(m_width * m_height * 4, 0);
	}

	void SetPixel(s32 x, s32 y, u8 r, u8 g, u8 b, u8 a = 255)
	{
		if (x < 0 || x >= m_width || y < 0 || y >= m_height) 
			return;

		s32 idx = 4 * (y * m_width + x);
		m_pixels[idx + 0] = r;
		m_pixels[idx + 1] = g;
		m_pixels[idx + 2] = b;
		m_pixels[idx + 3] = a;
	}

	void UpdateTexture()
	{
		m_texture->Bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels.data());
		m_texture->Unbind();
	}

	void Draw()
	{
		m_texture->Bind();
		m_quad->Draw();
		m_texture->Unbind();
	}

	void Clear(Color c = { 0, 0, 0, 255 })
	{
		for (s32 x = 0; x < m_width; ++x)
		{
			for (s32 y = 0; y < m_height; ++y)
			{
				SetPixel(x, y, c.r, c.g, c.b, c.a);
			}
		}
	}
};