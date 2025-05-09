/*
	Flow Field
	Perlin Noise
	Particle System
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

	mf4x4 proj;
	std::unique_ptr<Shader> shader = nullptr;
	std::unique_ptr<Shader> circle_shader = nullptr;

	struct Particle
	{
		vf2 pos;
		vf2 vel;
		vf4 color;
		vf2 acc;
	};

	std::vector<Particle> particles;

	s32 w;
	s32 h;
	s32 scale = 30; // 10 20 40 60
	std::vector<std::unique_ptr<line>> vector_lines;
	std::vector<vf3> flowfield;
	f32 length = 20.0f;
	f32 z = 0.0f;
	f32 max_speed = 80.0f;

	// Particle
	s32 n_particles = 500;
	u32 VAO, VBO;
	f32 particle_size = 6.0f;

	// RNG and Noise
	Random rng;
	FastNoiseLite noise;
	f32 noise_frequency = 0.80f;
	s32 noise_octaves = 6;
	f32 noise_lacunarity = 2.0f;
	f32 noise_gain = 0.5f;
	f32 noise_scale = 1.00f;

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
		s32 cols = std::floor(w / scale);
		s32 rows = std::floor(h / scale);
		for (s32 y = 0; y < rows; y++)
		{
			for (s32 x = 0; x < cols; x++)
			{
				f32 angle = noise.GetNoise(static_cast<f32>(x * noise_scale), static_cast<f32>(y * noise_scale), z) * TAU;
				vf3 dir = { std::cosf(angle), std::sinf(angle), 0.0f };
				//vf3 dir = curl(x, y, z);
				//dir = glm::normalize(dir) * 0.10f;
				flowfield.push_back(dir);

				vf3 pos = { static_cast<f32>(x * scale), static_cast<f32>(y * scale) , 0.0f };
				vf3 end = pos + dir * length;

				vector_lines.push_back(std::make_unique<line>(pos, end));
			}
		}

	}

	void generate_particles()
	{
		particles.clear();
		for (s32 i = 0; i < n_particles; i++)
		{
			Particle p;
			//f32 x = rng.uniform(0.0f, w);
			//f32 y = rng.uniform(0.0f, h);
			f32 x = rng.normal(w /2, w / 8);
			f32 y = rng.normal(h /2, h / 8);

			p.pos = { x, y };
			p.vel = { 0.0f, 0.0f };
			p.color = { 0.875f, 0.01f, 0.01f, 0.025f };
			p.acc = { 0.0f, 0.0f };
			particles.push_back(p);
		}
	}

	// finite difference
	vf3 curl(f32 x, f32 y, f32 z, f32 eps = 0.001f) 
	{
		f32 n_y0 = noise.GetNoise(x, y + eps, z);
		f32 n_y1 = noise.GetNoise(x, y - eps, z);
		f32 n_x0 = noise.GetNoise(x + eps, y, z);
		f32 n_x1 = noise.GetNoise(x - eps, y, z);

		f32 dfdx = (n_x0 - n_x1) / (2 * eps);
		f32 dfdy = (n_y0 - n_y1) / (2 * eps);

		return vf3{ dfdy, -dfdx, 0.0f };
	}


public:
	void Create() override
	{
		m_gui.show_fps = false;

		shader        = std::make_unique<Shader>("res/shaders/basic/default.vs", "res/shaders/basic/default.fs");
		circle_shader = std::make_unique<Shader>("res/shaders/basic/circle.vs", "res/shaders/basic/circle.fs");

		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		//noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		//noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
		noise.SetSeed(1337);
		noise.SetFrequency(noise_frequency);
		
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		noise.SetFractalOctaves(noise_octaves);
		noise.SetFractalLacunarity(noise_lacunarity);
		noise.SetFractalGain(noise_gain);

		w = m_window.Width();
		h = m_window.Height();

		proj = glm::ortho(0.0f, f32(w), 0.0f, f32(h), 0.1f, -1.0f);

		// Flow Field Grid
		generate_flowfield();

		// Particles
		generate_particles();

		// push particle to GPU
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles.size(), particles.data(), GL_DYNAMIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, vel));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
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
			// Update Flow Field
			s32 idx = 0;
			flowfield.clear();
			s32 cols = std::floor(w / scale);
			s32 rows = std::floor(h / scale);
			for (s32 y = 0; y < rows; y++)
			{
				for (s32 x = 0; x < cols; x++)
				{
					f32 angle = noise.GetNoise(static_cast<f32>(x * noise_scale), static_cast<f32>(y * noise_scale), z) * TAU;
					vf3 dir = { std::cosf(angle), std::sinf(angle), 0.0f };
					//vf3 dir = curl(x, y, z);
					//dir = glm::normalize(dir) * 0.1f;
					flowfield.push_back(dir);
					
					vf3 pos = { static_cast<f32>(x * scale), static_cast<f32>(y * scale), 0.0f };
					vf3 end = pos + dir * length;
					update_line(vector_lines[idx].get(), pos, end);
					idx++;
				}
			}
			z += dt * 100.0f;
		}

		if (draw_particles)
		{
			// Update Particles
			for (auto& p : particles)
			{
				s32 cols = std::floor(w / scale);
				s32 rows = std::floor(h / scale);

				s32 gx = s32(p.pos.x / scale);
				s32 gy = s32(p.pos.y / scale);

				gx = std::clamp(gx, 0, cols - 1);
				gy = std::clamp(gy, 0, rows - 1);

				s32 idx = gy * cols + gx;

				p.acc = flowfield[idx];
				p.vel += p.acc;

				// Clamp velocity
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
			circle_shader->SetUniform("size", particle_size);
			circle_shader->SetUniform("max_speed", max_speed);

			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, (GLsizei)particles.size());
			glBindVertexArray(0);
		}

		m_gui.m_func = [&]() {
			ImGui::Begin("Flow Field Parameters");
			ImGui::SliderFloat("Noise Scale",        &noise_scale, 0.01f, 2.0f); // larger the value, erratic the flow
			ImGui::SliderFloat("Frequency",          &noise_frequency,   0.0f, 2.0f); // larger the value, erratic the flow
			ImGui::SliderInt("Octaves",              &noise_octaves,     1, 20);
			ImGui::SliderFloat("Lacunarity",         &noise_lacunarity,  0.0f, 4.0f);
			ImGui::SliderFloat("Gain",               &noise_gain,        0.0f, 1.0f);
			ImGui::SliderFloat("Particle Size",      &particle_size,     0.01f, 20.0f);
			ImGui::SliderFloat("Particle Max Speed", &max_speed,         0.00f, 200.0f);
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