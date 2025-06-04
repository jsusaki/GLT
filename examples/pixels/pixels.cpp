/*
	Pixels
		Simple example of using Sprite class to create 
		an empty texture, filling pixel buffer for GPU rendering
*/
#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

class Pixels : public Application
{
public:
	Pixels() {}

public:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;
	Random rng;
	s32 scale = 4;

	void Create() override
	{
		// Create empty texture
		sprite = std::make_unique<Sprite>(m_window.Width(), m_window.Height());
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{
		for (s32 x = 0; x < m_window.Width(); x++)
		{
			for (s32 y = 0; y < m_window.Height(); y++)
			{
				u8 r = rng.uniformi(0, 255);
				u8 g = rng.uniformi(0, 255);
				u8 b = rng.uniformi(0, 255);

				s32 sx = x * scale;
				s32 sy = y * scale;

				for (s32 dx = 0; dx < scale; dx++)
				{
					for (s32 dy = 0; dy < scale; dy++)
					{
						sprite->SetPixel(sx + dx, sy + dy, r, g, b);
					}
				}
			}
		}

		sprite->UpdateTexture();
	}

	void Render() override
	{
		m_window.Clear();

		texture_shader->Use();
		texture_shader->SetUniform("screen_texture", 0);

		sprite->Draw();
	}
};

int main()
{
	Pixels demo;
	if (demo.Init("Pixels", 800, 600))
		demo.Start();
}
