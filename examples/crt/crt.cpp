#include "Application.h"

#include "Graphics/Texture.h"
#include "Graphics/Quad.h"
#include "Graphics/Shader.h"

class Experiment : public Application
{
public:
	Experiment() {}

public:
	void Create() override
	{
		tex       = new Texture("res/images/wizardRL.png");
		quad      = new Quad();
		shader    = new Shader("res/shaders/post_processing/post_processing.vs", "res/shaders/post_processing/simple_crt.fs");

		 config = {
			.scanline_amplitude = 0.3f, //0.19f
			.scanline_frequency = 1920.0f,
			.scanline_offset = 1.20f,
			.primary_curvature = 0.0125f,   //0.01f
			.secondary_curvature = 0.0f,
			.vignette_radius = 0.8f,
			.vignette_softness = 0.25f,
			.blur_radius = 0.003f,
			.blend_factor = 0.2f,
			.gamma = 0.8f,
			.contrast = 1.1f,      // 1.0f
			.saturation = 1.15f,   // 1.0f
			.brightness = 1.10f,   // 1.0f
			.color_correction = 1.0f,

			.phosphor_dot_scale = 640.0f,
			.phosphor_dot_softness = 0.15f,

			.bloom_intensity = 0.5f,
			.bloom_threshold = 0.15f,
			.bloom_blend_factor = 0.5f,
		};
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{

	}

	void Render() override
	{
		shader->Use();
		shader->SetUniform("screen_texture", 0);

		// Shader Uniforms
		shader->SetUniform("screen_resolution",     { m_window.Width(), m_window.Height() });
		shader->SetUniform("scanline_amplitude",    config.scanline_amplitude);
		shader->SetUniform("scanline_frequency",    config.scanline_frequency);
		shader->SetUniform("scanline_offset",       config.scanline_offset);
		shader->SetUniform("primary_curvature",     config.primary_curvature);
		shader->SetUniform("secondary_curvature",   config.secondary_curvature);
		shader->SetUniform("vignette_radius",       config.vignette_radius);
		shader->SetUniform("vignette_softness",     config.vignette_softness);
		shader->SetUniform("blur_radius",           config.blur_radius);
		shader->SetUniform("blend_factor",          config.blend_factor);
		shader->SetUniform("gamma",                 config.gamma);
		shader->SetUniform("contrast",              config.contrast);
		shader->SetUniform("saturation",            config.saturation);
		shader->SetUniform("brightness",            config.brightness);
		shader->SetUniform("color_correction",      config.color_correction);
		shader->SetUniform("phosphor_dot_scale",    config.phosphor_dot_scale);
		shader->SetUniform("phosphor_dot_softness", config.phosphor_dot_softness);
		shader->SetUniform("bloom_intensity",       config.bloom_intensity);
		shader->SetUniform("bloom_threshold",       config.bloom_threshold);
		shader->SetUniform("bloom_blend_factor",    config.bloom_blend_factor);

		// Draw Texture
		tex->Bind();
		quad->Draw();

		// Draw Interface
		m_gui.m_func = [&]() {
			ImGui::Begin("Simple CRT");
			ImGui::SeparatorText("CRT Effect Controls");
			ImGui::SliderFloat("scanline_amplitude",     &config.scanline_amplitude, 0.0, 1.0);
			ImGui::SliderFloat("scanline_frequency",     &config.scanline_frequency, 0.0, 3000.0);
			ImGui::SliderFloat("scanline_offset",        &config.scanline_offset, 0.0, 10.0);
			ImGui::SliderFloat("primary_curvature",      &config.primary_curvature, -2.0, 2.0);
			ImGui::SliderFloat("secondary_curvature",    &config.secondary_curvature, -2.0, 2.0);
			ImGui::SliderFloat("vignette_radius offset", &config.vignette_radius, 0.0, 1.0);
			ImGui::SliderFloat("vignette_softness",      &config.vignette_softness, 0.0, 1.0);
			ImGui::SliderFloat("blur_radius",            &config.blur_radius, 0.0, 1.0);
			ImGui::SliderFloat("blend_factor",           &config.blend_factor, 0.0, 1.0);
			ImGui::SliderFloat("gamma",                  &config.gamma, 0.1, 3.0);
			ImGui::SliderFloat("contrast",               &config.contrast, 0.5, 2.0);
			ImGui::SliderFloat("saturation",             &config.saturation, 0.0, 2.0);
			ImGui::SliderFloat("brightness",             &config.brightness, 0.0, 2.0);
			ImGui::SliderFloat("color_correction",       &config.color_correction, 0.0, 2.0);

			ImGui::SliderFloat("phosphor_dot_scale",     &config.phosphor_dot_scale, 0.0, 1280.0);
			ImGui::SliderFloat("phosphor_dot_softness",  &config.phosphor_dot_softness, 0.0, 1.0);

			ImGui::SliderFloat("bloom_intensity ",       &config.bloom_intensity, 0.0, 1.0);
			ImGui::SliderFloat("bloom_threshold ",       &config.bloom_threshold, 0.0, 1.0);
			ImGui::SliderFloat("bloom_blend_factor ",    &config.bloom_blend_factor, 0.0, 1.0);

			ImGui::End();
		};
	}

private:
	Quad* quad = nullptr;
	Texture* tex = nullptr;
	Shader* shader = nullptr;

	struct SimpleCRTConfig
	{
		float scanline_amplitude;
		float scanline_frequency;
		float scanline_offset;
		float primary_curvature;
		float secondary_curvature;
		float vignette_radius;
		float vignette_softness;
		float blur_radius;
		float blend_factor;
		float gamma;
		float contrast;
		float saturation;
		float brightness;
		float color_correction;

		float phosphor_dot_scale;
		float phosphor_dot_softness;

		float bloom_intensity;
		float bloom_threshold;
		float bloom_blend_factor;
	} config;
};


int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}