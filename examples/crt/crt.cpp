#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"
#include "Graphics/PostProcessor.h"

class CRT : public Application
{
public:
	CRT() {}
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> crt_shader;
	std::unique_ptr<Shader> texture_shader;
	std::unique_ptr<PostProcessor> post_processor;

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

		float phosphor_dot_scale = 640.0f;
		float phosphor_dot_softness = 0.15f;

		float bloom_intensity = 0.5f;
		float bloom_threshold = 0.15f;
		float bloom_blend_factor = 0.5f;
	} config;

public:
	void Create() override
	{
		sprite = std::make_unique<Sprite>("res/images/wizardRL.png");
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");
		crt_shader = std::make_unique<Shader>("res/shaders/post_processing/post_processing.vs", "res/shaders/post_processing/simple_crt.fs");
		post_processor = std::make_unique<PostProcessor>(m_window.Width(), m_window.Height());
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

		post_processor->Begin();

		texture_shader->Use();
		texture_shader->SetUniform("screen_texture", 0);

		// Draw Texture
		sprite->Draw();

		// Post Processing
		crt_shader->Use();
		crt_shader->SetUniform("screen_texture", 0);

		// Shader Uniforms
		crt_shader->SetUniform("screen_resolution",     { m_window.Width(), m_window.Height() });
		crt_shader->SetUniform("scanline_amplitude",    config.scanline_amplitude);
		crt_shader->SetUniform("scanline_frequency",    config.scanline_frequency);
		crt_shader->SetUniform("scanline_offset",       config.scanline_offset);
		crt_shader->SetUniform("primary_curvature",     config.primary_curvature);
		crt_shader->SetUniform("secondary_curvature",   config.secondary_curvature);
		crt_shader->SetUniform("vignette_radius",       config.vignette_radius);
		crt_shader->SetUniform("vignette_softness",     config.vignette_softness);
		crt_shader->SetUniform("blur_radius",           config.blur_radius);
		crt_shader->SetUniform("blend_factor",          config.blend_factor);
		crt_shader->SetUniform("gamma",                 config.gamma);
		crt_shader->SetUniform("contrast",              config.contrast);
		crt_shader->SetUniform("saturation",            config.saturation);
		crt_shader->SetUniform("brightness",            config.brightness);
		crt_shader->SetUniform("color_correction",      config.color_correction);
		crt_shader->SetUniform("phosphor_dot_scale",    config.phosphor_dot_scale);
		crt_shader->SetUniform("phosphor_dot_softness", config.phosphor_dot_softness);
		crt_shader->SetUniform("bloom_intensity",       config.bloom_intensity);
		crt_shader->SetUniform("bloom_threshold",       config.bloom_threshold);
		crt_shader->SetUniform("bloom_blend_factor",    config.bloom_blend_factor);

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


};

int main()
{
	CRT demo;
	if (demo.Init("Experiment", 1280, 960))
		demo.Start();
	return 0;
}