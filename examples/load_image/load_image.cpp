#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

class Image : public Application
{
public:
	Image() {}

public:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;

public:
	void Create() override
	{
		sprite = std::make_unique<Sprite>("res/images/wizardRL.png");
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{

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
	Image demo;
	if (demo.Init("Load Image Texture", 1280, 960))
		demo.Start();
	return 0;
}