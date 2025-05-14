#pragma once

#include "Graphics/Texture.h"
#include "Graphics/TextureQuad.h"

struct Sprite
{
	std::unique_ptr<TextureQuad> texture_quad;
	std::unique_ptr<Texture> texture;

	Sprite(const std::string& filepath)
	{
		texture = std::make_unique<Texture>(filepath);
		texture_quad = std::make_unique<TextureQuad>();
	}

	void Draw()
	{
		texture->Bind();
		texture_quad->Draw();
	}
};