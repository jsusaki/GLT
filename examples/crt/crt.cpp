#include "Application.h"

#include "Graphics/Texture.h"
#include "Graphics/Quad.h"
#include "Graphics/Shader.h"
#include "Graphics/PostProcessor.h"

class Experiment : public Application
{
public:
	Experiment() {}

public:
	void Create() override
	{
		tex = new Texture("res/images/wizardRL.png");
		quad = new Quad();

		basic = new Shader(
			"res/shaders/basic/basic.vs",
			"res/shaders/basic/basic.fs"
		);

		shader = new Shader(
			"res/shaders/post_processing/post_processing.vs", 
			"res/shaders/post_processing/simple_crt.fs"
		);

		post_processor = new PostProcessor(m_window.Width(), m_window.Height());
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{

	}

	void Render() override
	{
		post_processor->Begin();

		basic->Use();
		basic->SetUniform("screen_texture", 0);

		// Draw Texture
		tex->Bind();
		quad->Draw();

		// Post Processing
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

		post_processor->End();
		post_processor->Render();

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
	Shader* basic = nullptr;
	PostProcessor* post_processor = nullptr;

	struct SimpleCRTConfig
	{
		float scanline_amplitude = 0.3f;
		float scanline_frequency = 1920.0f;
		float scanline_offset = 1.20f;
		float primary_curvature = 0.0125f;
		float secondary_curvature = 0.0f;
		float vignette_radius = 0.8f;
		float vignette_softness = 0.25f;
		float blur_radius = 0.003f;

		float blend_factor = 0.2f;
		float gamma = 0.8f;
		float contrast = 1.1f;
		float saturation = 1.15f;
		float brightness = 1.10f;
		float color_correction = 1.0f;

		float phosphor_dot_scale    = 640.0f;
		float phosphor_dot_softness = 0.15f;

		float bloom_intensity = 0.5f;
		float bloom_threshold = 0.15f;
		float bloom_blend_factor = 0.5f;
	} config;
};

int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}