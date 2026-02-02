#include "Application.h"

#include "Graphics/Shader.h"
#include "Graphics/Sprite.h"

class Prisma : public Application
{
public:
	Prisma() {}

public:
	std::unique_ptr<Shader> prisma_shader;
	std::unique_ptr<Sprite> sprite;
	vf2 screen_size;
	f32 time = 0.0f;

public:
	void Create() override
	{
		screen_size = { m_window.Width(), m_window.Height() };
		sprite = std::make_unique<Sprite>(screen_size.x, screen_size.y);
		prisma_shader = std::make_unique<Shader>("res/shaders/prisma/prisma.vs", "res/shaders/prisma/prisma.fs");
	}

	void ProcessInput() override 
	{
	
	}

	void Simulate(f32 dt) override
	{
		time += dt;
	}

	void Render() override
	{
		m_window.Clear();
		
		prisma_shader->Use();
		prisma_shader->SetUniform("r", screen_size);
		prisma_shader->SetUniform("t", time);
		
		sprite->Draw();
	}

	void Destroy() override
	{

	}
};

int main()
{
	Prisma demo;
	if (demo.Init("Prisma", 1024, 1024))
		demo.Start();
	return 0;
}