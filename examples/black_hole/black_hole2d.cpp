/*
	Black Hole

		Method:
			Visual Effect Simulation

		Graphics:
			Gravitational Lensing

		Referenes:
			https://github.com/kavan010/black_hole
			https://github.com/rossning92/Blackhole
			https://github.com/nicolasloizeau/raytracing-black-holes

			Simulating a Black Hole!: https://www.asc.ohio-state.edu/orban.14/stemcoding/blackhole.html
			Physics of Black Hole:    https://oseiskar.github.io/black-hole/docs/physics.html
			How to draw a Black Hole: https://rantonels.github.io/starless/
			Visualizing Black Holes with General Relativistic Ray Tracing: https://blog.seanholloway.com/2022/03/13/visualizing-black-holes-with-general-relativistic-ray-tracing/
			Visualizing General Relativity: https://michaelmoroz.github.io/TracingGeodesics/

			Ray Tracing Black Holes:  https://eliot1019.github.io/Black-Hole-Raytracer/
			Implementing General Relativity: https://20k.github.io/c++/2024/05/31/schwarzschild.html
*/

#include <numbers>
#include <thread>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Application.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Sprite.h"


#include "Graphics/Camera.h"

class BlackHole : public Application
{
public:
	BlackHole() {}

	std::unique_ptr<Shader> black_hole_shader;

	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;

	struct ShaderConfig 
	{
		glm::vec2 bh_center = glm::vec2(0.5f, 0.5f);
		float schwarzschild_radius = 0.05f;
		float lensing_strength = 0.30f;
		float einstein_radius = 0.02f;
		float accretion_thickness = 0.02f;
		float accretion_intensity = 1.00f;
		float chroma_offset = 0.04f;
		float blur_strength = 0.32f;

		float photon_ring_intensity = 0.5f;
		float photon_ring_thickness = 0.01f;
	} config;

public:
	void Create() override
	{
		// Background Stars
		sprite            = std::make_unique<Sprite>("res/images/nasa.png");
		black_hole_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/black_hole/black_hole.fs");
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

		// Background Stars
		black_hole_shader->Use();
		black_hole_shader->SetUniform("screen_texture", 0);	
		black_hole_shader->SetUniform("bh_center",             config.bh_center);
		black_hole_shader->SetUniform("schwarzschild_radius",  config.schwarzschild_radius);
		black_hole_shader->SetUniform("lensing_strength",      config.lensing_strength);
		black_hole_shader->SetUniform("einstein_radius",       config.einstein_radius);
		black_hole_shader->SetUniform("accretion_thickness",   config.accretion_thickness);
		black_hole_shader->SetUniform("accretion_intensity",   config.accretion_intensity);
		black_hole_shader->SetUniform("chroma_offset",         config.chroma_offset);
		black_hole_shader->SetUniform("blur_strength",         config.blur_strength);
		black_hole_shader->SetUniform("photon_ring_intensity", config.photon_ring_intensity);
		black_hole_shader->SetUniform("photon_ring_thickness", config.photon_ring_thickness);
		sprite->Draw();
		black_hole_shader->Unuse();

		m_gui.m_func = [&]() {
			ImGui::Begin("Black Hole Shader");
			ImGui::SeparatorText("Black Hole Controls");
			ImGui::SliderFloat2("bh_center",           (float*)&config.bh_center,      0.0f, 1.0f);
			ImGui::SliderFloat("schwarzschild_radius",  &config.schwarzschild_radius,   0.0f, 1.0f);
			ImGui::SliderFloat("lensing_strength",      &config.lensing_strength,       0.0f, 1.0f);
			ImGui::SliderFloat("einstein_radius",       &config.einstein_radius,        0.0f, 0.1f);
			ImGui::SliderFloat("accretion_thickness",   &config.accretion_thickness,    0.0f, 1.0f);
			ImGui::SliderFloat("accretion_intensity",   &config.accretion_intensity,    0.0f, 3.0f);
			ImGui::SliderFloat("chroma_offset",         &config.chroma_offset,          0.0f, 1.0f);
			ImGui::SliderFloat("blur_strength",         &config.blur_strength,          0.0f, 1.0f);
			ImGui::SliderFloat("photon_ring_intensity", &config.photon_ring_intensity, 0.0f, 3.0f);
			ImGui::SliderFloat("photon_ring_thickness", &config.photon_ring_thickness, 0.0f, 0.1f);
			ImGui::End();
		};


	}
};

int main()
{
	BlackHole demo;
	if (demo.Init("BlackHole", 1280, 720))
		demo.Start();
	return 0;
}