/*
	Fluid Simulation

	References:
		https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
		https://mikeash.com/pyblog/fluid-simulation-for-dummies.html

		How to write an Eulerian fluid simulator with 200 lines of code
		https://www.youtube.com/watch?v=iKAVRgIrUOU&ab_channel=TenMinutePhysics
*/

#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

#include "Core/Random.h"
#include "FastNoiseLite/FastNoiseLite.h"

#include "fluid_simulation.h"

class FluidSimulator : public Application
{
public:
	FluidSimulator() {}

public:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;
	Random rng;

	vf2 mouse_pos = {};
	vf2 mouse_pos_prev = {};

	FluidModel* fluid;
	FastNoiseLite noise;
	vf2 position = { 400.0f / scale, 400.0f / scale };
	f32 noise_scale = 0.1f;
	f32 speed = 30.0f;
	bool update = true;

public:
	void Create() override
	{
		sprite = std::make_unique<Sprite>(m_window.Width(), m_window.Height());
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");

		fluid = new FluidModel(N, 0.2f, 0.0f, 0.0f);

		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetSeed(1337);
		noise.SetFrequency(0.1f);

		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		noise.SetFractalOctaves(3);
		noise.SetFractalLacunarity(0.5f);
		noise.SetFractalGain(0.1f);
	}

	void ProcessInput() override
	{
		// Start / Stop
		if (m_input.IsKeyPressed(GLFW_KEY_SPACE))
			update= !update;

		// Reset
		if (m_input.IsKeyPressed(GLFW_KEY_R))
		{
			for (s32 i = 0; i < fluid->density.size(); i++)
				fluid->density[i] = 0.0f;
		}

		f32 angle = noise.GetNoise(position.x / scale * noise_scale, position.y / scale * noise_scale) * TAU;
		vf2 direction = { std::cosf(angle), std::sinf(angle) };
		vf2 velocity = glm::normalize(direction) * speed;
		position += direction;
		position += rng.normal(0.0f, 0.05f);

		if (position.x < 0) position.x = m_window.Width() / scale;
		if (position.y < 0) position.y = m_window.Height() / scale;
		if (position.x > m_window.Width() / scale)  position.x = 0;
		if (position.y > m_window.Height() / scale) position.y = 0;

		fluid->AddDensity(fluid_cube, static_cast<s32>(position.x), static_cast<s32>(position.y), rng.uniformi(100, 150));
		fluid->AddVelocity(fluid_cube, static_cast<s32>(position.x), static_cast<s32>(position.y), velocity.x, velocity.y);
	}

	void Simulate(f32 dt) override
	{
		if (update)
		{
			mouse_pos = m_input.GetMouse();
			vf2 current  = vf2(mouse_pos.x / scale, (m_window.Height() - mouse_pos.y) / scale);
			vf2 previous = vf2(mouse_pos_prev.x / scale, (m_window.Height() - mouse_pos_prev.y) / scale);
			vf2 velocity = (current - previous) * 0.01f;
			
			if (m_input.IsButtonPressed(GLFW_MOUSE_BUTTON_1))
				fluid->AddDensity(static_cast<s32>(current.x), static_cast<s32>(current.y), rng.uniformi(25, 150));
			
			fluid->AddVelocity(static_cast<s32>(current.x), static_cast<s32>(current.y), velocity.x, velocity.y);
			mouse_pos_prev = mouse_pos;

			/*
			s32 cx = s32((0.5f * m_window.Width()) / scale);
			s32 cy = s32((0.10f * m_window.Height()) / scale);
			for (s32 i = -1; i <= 1; i++) 
			{
				for (s32 j = -1; j <= 1; j++) 
				{
					fluid->AddDensity(fluid_cube, cx + i, cy + j, rng.uniformi(50, 150));
				}
			}
			fluid->AddVelocity(fluid_cube, cx, cy, 0.0f, 0.0005f);
			*/

			fluid->Simulate(dt*speed);

			// Update Pixel Buffer
			for (s32 x = 0; x < N; x++)
			{
				for (s32 y = 0; y < N; y++)
				{
					f32 d = fluid->density[idx(x, y)];
					Color c = GetColor(d, 0.0f, 150.0f);

					s32 sx = x * scale;
					s32 sy = y * scale;

					for (s32 dx = 0; dx < scale; dx++)
					{
						for (s32 dy = 0; dy < scale; dy++)
						{
							sprite->SetPixel(sx + dx, sy + dy, c.r, c.g, c.b, std::clamp(d, 0.0f, 1.0f) * 255);
						}
					}
				}
			}

			sprite->UpdateTexture();
		}
	}

	void Render() override
	{
		m_window.Clear();

		texture_shader->Use();
		texture_shader->SetUniform("screen_texture", 0);

		sprite->Draw();

		m_gui.show_gui = false;
		m_gui.show_fps = true;
		m_gui.m_func = [&]() {
			ImGui::Begin("Parameters");
			ImGui::End();
		};
	}
};

int main()
{
	FluidSimulator demo;
	if (demo.Init("Fluid Simulator", 1024, 1024))
		demo.Start();
}