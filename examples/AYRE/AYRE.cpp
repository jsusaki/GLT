/*
	Armored Core 6 - Coral Contact: Ayre

	https://youtu.be/EgqUkCTS-Yc?t=116

	https://armoredcore.fandom.com/wiki/Ayre

	Scene sequence description
		1. black background
		2. small center red dot intensifying slowly
		2. small center red dot splash sparking network
		3. small to medium center red dot with glint sparking
		4. large center red dot with glint unfold into a network

	- random number generation techniques
	- noise techniques (seed)
	- network propagation techniques (growing)
	- interpolation techniques
	- glint techniques
	- color gradient techniques

	Particle System
	Perlin Noise
	Flow Field
*/

#include "Application.h"

#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
// #include "Graphics/Quad.h"
#include "Graphics/Shader.h"
#include "Graphics/PostProcessor.h"

#include "Core/Random.h"
#include "FastNoiseLite/FastNoiseLite.h"

class Experiment : public Application
{
public:
	Experiment() {}

	Shader* post_shader = nullptr;
	Shader* shader = nullptr;
	PostProcessor* post_processor = nullptr;

	f32 acc_time;

	struct Particle
	{
		vf2 pos;
		vf2 vel;
	};

	std::vector<Particle> particles;

	struct Point
	{
		vf2 pos;
		vf2 dir;
	};

	std::vector<Point> vector_field;
	std::vector<std::unique_ptr<Mesh>> vector_lines;

	Random rng;
	FastNoiseLite noise;

	Mesh* a_mesh = nullptr;

public:
	void Create() override
	{
		shader = new Shader("res/shaders/basic/basic.vs", "res/shaders/basic/basic.fs");

		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

		// Grid
		s32 w = m_window.Width();
		s32 h = m_window.Height();

		const s32 step = 20;
		const f32 length = 10.0f;
		for (s32 x = 0; x < w; x += step)
		{
			for (s32 y = 0; y < h; y += step)
			{
				vf3 pos = { static_cast<f32>(x), static_cast<f32>(y), 0.0f };
				f32 angle = noise.GetNoise(static_cast<f32>(x), static_cast<f32>(y));
				vf3 dir = { std::cosf(angle), std::sinf(angle), 0.0f };

				vf3 end = pos + dir * length;

				vector_lines.push_back(std::make_unique<line>(pos, end));
			}
		}

		// Simple line
		/*
		std::vector<vertex> vertices = {
			{{-0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // start
			{{ 0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}   // end
		};
		std::vector<u32> indices = { 0, 1 };
		a_mesh = new mesh(vertices, indices);
		*/

		/*
		// Triangle vertices
		std::vector<vertex> vertices = {
			{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } }
		};
		std::vector<u32> indices = { 0, 1, 2 };
		a_line = new mesh(vertices, indices);
		*/

	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{
		acc_time += dt;
	}

	void Render() override
	{
		shader->Use();
		a_mesh->draw(GL_LINES);
		//a_line->draw(GL_TRIANGLES);

		//a_line->draw(GL_LINES);
		//for (const auto& l : vector_lines) l->draw(GL_LINES);

		// GUI Code
		m_gui.m_func = [&]() {

		};
	}
};

/*
int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 720))
		exp.Start();
	return 0;
}
*/