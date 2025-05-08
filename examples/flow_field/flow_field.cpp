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
#include "Graphics/Shader.h"
#include "Graphics/PostProcessor.h"

#include "Core/Random.h"
#include "FastNoiseLite/FastNoiseLite.h"

class FlowField : public Application
{
public:
	FlowField() {}

	Shader* shader = nullptr;
	Shader* circle_shader = nullptr;

	struct Particle
	{
		vf2 pos;
		vf2 vel;
		vf2 acc;
	};

	std::vector<Particle> particles;
	std::vector<std::unique_ptr<line>> vector_lines;
	std::vector<vf3> flowfield;
	f32 length = 20.0f;
	f32 z = 0.0f;
	s32 w;
	s32 h;
	s32 cols = 96; // 96 128 256
	f32 max_speed = 100.0f;

	// particle
	const s32 N = 500;
	u32 VAO, VBO;
	f32 size = 6.0f;

	// RNG and Noise
	Random rng;
	FastNoiseLite noise;
	f32 noise_frequency = 0.80f;
	s32 noise_octaves = 6;

	mf4x4 proj;


	bool update = false;
	bool draw_lines = false;
	bool draw_particles = true;
	bool clear = false;

	void update_line(line* l, const vf3& start, const vf3& end)
	{
		l->vertices[0].position = start;
		l->vertices[1].position = end;

		glBindBuffer(GL_ARRAY_BUFFER, l->vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(vertex), l->vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void generate_flowfield()
	{
		flowfield.clear();
		const s32 step = w / cols;
		for (s32 y = 0; y < h; y += step)
		{
			for (s32 x = 0; x < w; x += step)
			{
				vf3 pos = { static_cast<f32>(x), static_cast<f32>(y), 0.0f };
				f32 angle = noise.GetNoise(static_cast<f32>(x), static_cast<f32>(y), z) * TAU;

				vf3 dir = { std::cosf(angle), std::sinf(angle), 0.0f };
				dir = glm::normalize(dir) * 0.50f;

				vf3 end = pos + dir * length;
				flowfield.push_back(dir);

				vector_lines.push_back(std::make_unique<line>(pos, end));
			}
		}
	}

	void generate_particles()
	{
		particles.clear();
		for (s32 i = 0; i < N; i++)
		{
			Particle p;
			//f32 rx = rng.uniform(0.0f, w);
			//f32 ry = rng.uniform(0.0f, h);

			f32 rx = rng.normal(w /2, w / 8);
			f32 ry = rng.normal(h /2, h / 8);

			p.pos = { rx, ry };
			p.vel = { 0.0f, 0.0f };
			p.acc = { 0.0f, 0.0f };
			particles.push_back(p);
		}
	}

public:
	void Create() override
	{
		m_gui.show_fps = false;

		shader = new Shader("res/shaders/basic/default.vs", "res/shaders/basic/default.fs");
		circle_shader = new Shader("res/shaders/basic/circle_sprite.vs", "res/shaders/basic/circle_sprite.fs");

		//noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetFrequency(noise_frequency);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		noise.SetFractalOctaves(noise_octaves);


		w = m_window.Width();
		h = m_window.Height();

		proj = glm::ortho(0.0f, f32(w), 0.0f, f32(h), 0.1f, -1.0f);

		// Flow Field Grid
		generate_flowfield();

		// Particles
		generate_particles();
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * N, nullptr, GL_DYNAMIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));
		glBindVertexArray(0);

		glEnable(GL_PROGRAM_POINT_SIZE);

	}

	void ProcessInput() override
	{
		update = false;
		if(m_input.IsKeyPressed(GLFW_KEY_SPACE))
			update = true;

		if (m_input.IsKeyPressed(GLFW_KEY_E))
			clear = !clear;

		if (m_input.IsKeyPressed(GLFW_KEY_TAB))
			m_gui.show_gui = !m_gui.show_gui;

		if (m_input.IsKeyPressed(GLFW_KEY_R))
		{
			generate_flowfield();
			generate_particles();
		}

		if (m_input.IsKeyPressed(GLFW_KEY_Q))
			draw_lines = !draw_lines;

		if (m_input.IsKeyPressed(GLFW_KEY_W))
			draw_particles = !draw_particles;

	}

	void Simulate(f32 dt) override
	{
		noise.SetFrequency(noise_frequency);

		if (update)
		{
			// Flow Field
			const s32 step = w / cols;
			s32 idx = 0;
			flowfield.clear();
			for (s32 y = 0; y < h; y += step)
			{
				for (s32 x = 0; x < w; x += step)
				{
					vf3 pos = { static_cast<f32>(x), static_cast<f32>(y), 0.0f };
					f32 angle = noise.GetNoise(static_cast<f32>(x), static_cast<f32>(y), z) * TAU;
					vf3 dir = { std::cosf(angle), std::sinf(angle), 0.0f };
					dir = glm::normalize(dir) * 0.1f;
					vf3 end = pos + dir * length;
					flowfield.push_back(dir);
					update_line(vector_lines[idx].get(), pos, end);
					idx++;
				}
			}
			z += dt * 100.0f;
		}

		// Particles
		for (auto& p : particles) 
		{
			s32 idx = std::floor(p.pos.y / cols) * (w / cols) + std::floor(p.pos.x / cols);
			p.acc = flowfield[idx];
			p.vel += p.acc;

			// Clamp
			if (glm::length(p.vel) > max_speed)
				p.vel = glm::normalize(p.vel) * max_speed;

			p.pos += p.vel * dt;
			p.acc *= 0.0f;

			if (p.pos.x < 0) p.pos.x = w;;
			if (p.pos.x > w) p.pos.x = 0;
			if (p.pos.y < 0) p.pos.y = h;
			if (p.pos.y > h) p.pos.y = 0;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * particles.size(), particles.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Render() override
	{
		if (clear) 
		{
			glm::vec4 v = to_float({ 0,0,0,0 });
			glClearColor(v.r, v.g, v.b, v.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		if (draw_lines)
		{
			shader->Use();
			shader->SetUniform("projection", proj);
			for (const auto& l : vector_lines)
				l->draw(GL_LINES);
		}

		if (draw_particles)
		{
			circle_shader->Use();
			circle_shader->SetUniform("projection", proj);
			circle_shader->SetUniform("color", glm::vec4({ 0.875f, 0.01f, 0.01f ,0.025f }));
			circle_shader->SetUniform("size", size);

			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, (GLsizei)particles.size());
			glBindVertexArray(0);
		}

		m_gui.m_func = [&]() {
			ImGui::Begin("Flow Field Parameters");
			ImGui::SliderFloat("Frequency", &noise_frequency, 0.0f, 1.0f);
			ImGui::SliderInt("Octaves",     &noise_octaves,   1, 20);
			ImGui::SliderFloat("Size",      &size,            0.01f, 20.0f);
			ImGui::SliderFloat("Max Speed", &max_speed,       0.00f, 200.0f);
			ImGui::End();
		};
	}
};

int main()
{
	FlowField exp;
	if (exp.Init("Flow Field", 1280, 720))
		exp.Start();
	return 0;
}