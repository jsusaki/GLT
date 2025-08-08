/*
	Fourier Transform
		FFT
		Sound Wave
		Audio Visualizer?

	Ref:
		https://github.com/JanKleprlik/AudioVisualiser/tree/master
		https://github.com/projectM-visualizer/projectM
*/
#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

class FourierTransform : public Application
{
public:
	FourierTransform() {}

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
	FourierTransform demo;
	if (demo.Init("FourierTransform", 800, 600))
		demo.Start();
}
