#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

#include "Core/Random.h"
#include "FastNoiseLite/FastNoiseLite.h"

class Noise : public Application
{
public:
	Noise() {}

	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;

	FastNoiseLite noise;
	s32 noise_type = 0;
	s32 noise_seed = 1337;
	f32 noise_frequency = 0.01f;

	s32 noise_fractal_type = 0;
	s32 noise_octaves = 3;
	f32 noise_lacunarity = 2.0f;
	f32 noise_gain = 0.5f;
	f32 noise_scale = 1.0f;
	f32 noise_fractal_weighted_strength = 0.0f;
	f32 noise_fractal_ping_pong_strength = 2.0f;

	s32 noise_cellular_type = 1;
	s32 noise_cellular_return_type = 1;
	f32 noise_cellular_jitter = 1.0f;

	FastNoiseLite noise_warp;
	s32 noise_domain_warp_seed = 1337;
	f32 noise_domain_warp_frequency = 0.01f;
	s32 noise_domain_warp_type = 0;
	s32 noise_domain_warp_rotationType = 0;
	f32 noise_domain_warp_amplitude = 1.0f;

	s32 noise_domain_warp_fractal_type = 0;
	s32 noise_domain_warp_fractal_octaves = 3;
	f32 noise_domain_warp_fractal_lacunarity = 2.0f;
	f32 noise_domain_warp_fractal_gain = 0.5f;

	bool texture_update = false;

	void SampleNoise()
	{
		for (s32 x = 0; x < m_window.Width(); x++)
		{
			for (s32 y = 0; y < m_window.Height(); y++)
			{
				u8 c = (noise.GetNoise(static_cast<f32>(x * noise_scale), static_cast<f32>(y * noise_scale)) + 1.0f) * 0.5f * 255;
				sprite->SetPixel(x, y, c, c, c);
			}
		}

		sprite->UpdateTexture();
	}

	void Create() override
	{
		// Create empty texture
		sprite = std::make_unique<Sprite>(m_window.Width(), m_window.Height());
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");
		
		// Sample noise and fill texture buffer
		SampleNoise();
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{
		if (texture_update)
		{
			SampleNoise();
		}

		texture_update = false;
	}

	void Render() override
	{
		m_window.Clear();

		texture_shader->Use();
		texture_shader->SetUniform("screen_texture", 0);

		sprite->Draw();

		m_gui.m_func = [&]() {
			static const char* enum_noise_type[]               = { "OpenSimplex2", "OpenSimplex2S", "Cellular", "Perlin", "Value Cubic", "Value" };
			static const char* enum_fractal_type[]             = { "None", "FBm", "Ridged", "Ping Pong" };
			static const char* enum_cellular_type[]            = { "Euclidean", "Euclidean Sq", "Manhattan", "Hybrid" };
			static const char* enum_cellular_return_type[]     = { "Cell Value", "Distance", "Distance 2", "Distance 2 Add", "Distance 2 Sub", "Distance 2 Mul", "Distance 2 Div" };
			static const char* enum_domain_warp_type[]         = { "None", "OpenSimplex2", "OpenSimplex2 Reduced", "Basic Grid" };
			static const char* enum_domain_warp_fractal_type[] = { "None", "Progressive", "Independent" };

			ImGui::Begin("Noise Parameters");

			// General
			ImGui::TextUnformatted("General");
			if (ImGui::Combo("Noise Type", &noise_type, enum_noise_type, IM_ARRAYSIZE(enum_noise_type)))
			{
				noise.SetNoiseType((FastNoiseLite::NoiseType)noise_type);
				texture_update = true;
			}
			if (ImGui::DragInt("Seed", &noise_seed))
			{
				noise.SetSeed(noise_seed);
				texture_update = true;
			}
			if (ImGui::DragFloat("Frequency", &noise_frequency, 0.0002f))
			{
				noise.SetFrequency(noise_frequency);
				texture_update = true;
			}
			if (ImGui::DragFloat("Noise Scale", &noise_scale, 0.01f, 2.0f))
			{
				texture_update = true;
			}

			// Fractal
			ImGui::TextUnformatted("Fractal");
			if (ImGui::Combo("Type", &noise_fractal_type, enum_fractal_type, IM_ARRAYSIZE(enum_fractal_type)))
			{
				noise.SetFractalType((FastNoiseLite::FractalType)noise_fractal_type);
				texture_update = true;
			}

			ImGui::BeginDisabled(noise_fractal_type == 0);
			if (ImGui::DragInt("Octaves", &noise_octaves, 0.1f, 1, 20))
			{
				noise.SetFractalOctaves(noise_octaves);
				texture_update = true;
			}

			if (ImGui::DragFloat("Lacunarity", &noise_lacunarity, 0.01f))
			{
				noise.SetFractalLacunarity(noise_lacunarity);
				texture_update = true;
			}

			if (ImGui::DragFloat("Gain", &noise_gain, 0.01f))
			{
				noise.SetFractalGain(noise_gain);
				texture_update = true;
			}

			if (ImGui::DragFloat("Weighted Strength", &noise_fractal_weighted_strength, 0.01f))
			{
				noise.SetFractalWeightedStrength(noise_fractal_weighted_strength);
				texture_update = true;
			}

			ImGui::BeginDisabled(noise_fractal_type != (s32)FastNoiseLite::FractalType_PingPong);
			if (ImGui::DragFloat("Ping Pong Weighted Strength", &noise_fractal_ping_pong_strength, 0.0f, 1.0f))
			{
				noise.SetFractalPingPongStrength(noise_fractal_ping_pong_strength);
				texture_update = true;
			}
			ImGui::EndDisabled();
			ImGui::EndDisabled();

			// Cellular
			ImGui::TextUnformatted("Cellular");
			ImGui::BeginDisabled(noise_type!= (s32)FastNoiseLite::NoiseType_Cellular);
			if (ImGui::Combo("Distance Function", &noise_cellular_type, enum_cellular_type, IM_ARRAYSIZE(enum_cellular_type)))
			{
				noise.SetCellularDistanceFunction((FastNoiseLite::CellularDistanceFunction)noise_cellular_type);
				texture_update = true;
			}
			if (ImGui::Combo("Return Type", &noise_cellular_return_type, enum_cellular_return_type, IM_ARRAYSIZE(enum_cellular_return_type)))
			{
				noise.SetCellularReturnType((FastNoiseLite::CellularReturnType)noise_cellular_return_type);
				texture_update = true;
			}
			if (ImGui::DragFloat("Jitter", &noise_cellular_jitter, 0.01f))
			{
				noise.SetCellularJitter(noise_cellular_jitter);
				texture_update = true;
			}
			ImGui::EndDisabled();

			// Domain Warp
			ImGui::TextUnformatted("Domain Warp");
			if (ImGui::Combo("Domain Warp Type", &noise_domain_warp_type, enum_domain_warp_type, IM_ARRAYSIZE(enum_domain_warp_type)))
			{
				noise_warp.SetDomainWarpType((FastNoiseLite::DomainWarpType)(noise_domain_warp_type - 1));
				texture_update = true;
			}
			if (ImGui::DragFloat("Domain Warp Amplitude", &noise_domain_warp_amplitude, 0.5f))
			{
				noise_warp.SetDomainWarpAmp(noise_domain_warp_amplitude);
				texture_update = true;
			}
			if (ImGui::DragInt("Domain Warp Seed", &noise_domain_warp_seed))
			{
				noise_warp.SetSeed(noise_domain_warp_seed);
				texture_update = true;
			}
			if (ImGui::DragFloat("Domain Warp Frequency", &noise_domain_warp_frequency, 0.001f))
			{
				noise_warp.SetFrequency(noise_domain_warp_frequency);
				texture_update = true;
			}

			// Domain Warp Fractal
			ImGui::TextUnformatted("Domain Warp Fractal");

			if (ImGui::Combo("Domain Warp Fractal Type", &noise_domain_warp_fractal_type, enum_domain_warp_fractal_type, IM_ARRAYSIZE(enum_domain_warp_fractal_type)))
			{
				noise_warp.SetFractalType((FastNoiseLite::FractalType)(noise_domain_warp_fractal_type ? noise_domain_warp_fractal_type+ 3 : 0));
				texture_update = true;
			}
			ImGui::BeginDisabled(noise_domain_warp_fractal_type == 0);
			if (ImGui::DragInt("Domain Warp Fractal Octaves", &noise_domain_warp_fractal_octaves, 0.1f, 1, 20))
			{
				noise_warp.SetFractalOctaves(noise_domain_warp_fractal_octaves);
				texture_update = true;
			}
			if (ImGui::DragFloat("Domain Warp Fractal Lacunarity", &noise_domain_warp_fractal_lacunarity, 0.01f))
			{
				noise_warp.SetFractalLacunarity(noise_domain_warp_fractal_lacunarity);
				texture_update = true;
			}
			if (ImGui::DragFloat("Domain Warp Fractal Gain", &noise_domain_warp_fractal_gain, 0.01f))
			{
				noise_warp.SetFractalGain(noise_domain_warp_fractal_gain);
				texture_update = true;
			}
			ImGui::EndDisabled();

			ImGui::End();
		};
	}
};

int main()
{
	Noise demo;
	if (demo.Init("Noise", 800, 600))
		demo.Start();
}
