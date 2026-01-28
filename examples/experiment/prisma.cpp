/*
	TODO: 
		Audio Reactive Visualization
		Load .wav
		DFT
		Frequency

		3D grid of audio reactive; like 3D wave table

*/
#include "Application.h"

#include "Graphics/Shader.h"
#include "Graphics/Sprite.h"

#include "soloud.h"
#include "soloud_wav.h"

class Prisma : public Application
{
public:
	Prisma() {}

	std::unique_ptr<Shader> prisma_shader;
	std::unique_ptr<Sprite> sprite;
	vf2 screen_size;
	f32 time = 0.0f;

	// SoLoud engine
	SoLoud::Soloud gSoloud;
	SoLoud::Wav gWave;

public:
	void Create() override
	{
		screen_size = { m_window.Width(), m_window.Height() };
		prisma_shader = std::make_unique<Shader>("res/shaders/experiment/prisma.vs", "res/shaders/experiment/prisma.fs");
		sprite = std::make_unique<Sprite>(screen_size.x, screen_size.y);

		// Init soloud
		gSoloud.init();
		gWave.load("res/audio/click.wav");
	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_P))
			gSoloud.play(gWave);
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
		gSoloud.deinit();
	}
};

int main()
{
	Prisma demo;
	if (demo.Init("Prisma", 1024, 1024))
		demo.Start();
	return 0;
}