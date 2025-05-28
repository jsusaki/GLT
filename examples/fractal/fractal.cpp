/*
	Fractal Explorer
		Mandelbrot Shader
		Julia Shader

	https://paulbourke.net/fractals/juliaset/
	https://www.karlsims.com/julia.html
*/
#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

#include "Core/Random.h"

class Fractal : public Application
{
public:
	Fractal() {}

	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> fractal_shader;

	f32 zoom = 0.9f;
	s32 max_iter = 85;
	f32 max_zoom = 100000.0f;
	vf2 c = { -0.7269f, 0.1889f };
	//c = {-0.943072290, 0.258960843};
	//c = {-0.772466100, 0.103424500};
	//c = {-0.743510000, 0.127090000};
	//c = {-0.79, 0.15};
	f32 angle = 0.0f;
	bool show_julia = true;
	bool animate_julia = false;
	f32 animation_speed = 0.12f;
	f32 timer = 0.0f;
	f32 interval = 10.0f;

	void Create() override
	{
		sprite         = std::make_unique<Sprite>(m_window.Width(), m_window.Height());
		fractal_shader = std::make_unique<Shader>("res/shaders/basic/fractal.vs", "res/shaders/basic/fractal.fs");
	}

	void ProcessInput() override
	{
		// Reset
		if (m_input.IsKeyPressed(GLFW_KEY_SPACE))
			zoom = 0.9f;
		
		// Animate
		if (m_input.IsKeyPressed(GLFW_KEY_TAB))
			animate_julia = !animate_julia;

		// Mouse Zoom
		// TODO: something is not working
		f32 wheel_delta = m_input.GetMouseWheel();
		if (wheel_delta > 0.0f)
			zoom += 0.01f;
		else if (wheel_delta < 0.0f)
			zoom -= 0.01f;

		zoom = glm::clamp(zoom, 0.0f, max_zoom);

		// Mouse Pan
	}
	
	void Simulate(f32 dt) override
	{
		if (animate_julia) 
		{
			f32 t = fmodf(timer, interval * 2.0f) / interval;
			f32 dir = t < 1.0f ? 1.0f : -1.0f;
			angle += dt * animation_speed * dir; 

			c = { std::cos(angle) + std::sin(angle * 3.0f) * 0.5f, std::sin(angle) };
			c *= 0.75f;
			//c = { std::cos(angle), std::sin(angle) };
		}
	}

	void Render() override
	{
		m_window.Clear();

		fractal_shader->Use();
		fractal_shader->SetUniform("screen_size", { m_window.Width(), m_window.Height() });
		fractal_shader->SetUniform("zoom",      zoom);
		fractal_shader->SetUniform("max_iter",   max_iter);
		fractal_shader->SetUniform("mouse",      m_input.GetMouse());
		fractal_shader->SetUniform("c",          c);
		fractal_shader->SetUniform("show_julia", show_julia);

		sprite->Draw();

		m_gui.show_gui = false;
		m_gui.m_func = [&]() {
			ImGui::Begin("Fractal Parameters");

			ImGui::DragFloat("Zoom",        &zoom,             0.01f, 0.1f, 100000.0f);
			ImGui::DragInt("Max Iteration", &max_iter,         1, 1, 5000);
			ImGui::DragFloat2("c",          glm::value_ptr(c), 0.001f, -1.0f, 1.0f);

			if (ImGui::Button(show_julia ? "Mandelbrot Set" : "Julia Set"))
				show_julia = !show_julia;

			ImGui::BeginDisabled(!show_julia);
			ImGui::Checkbox("Animate", &animate_julia);
			ImGui::DragFloat("Animation Speed", &animation_speed, 0.01f, -2.0f, 2.0f);
			ImGui::EndDisabled();

			ImGui::End();
		};
	}
};

int main()
{
	Fractal demo;
	if (demo.Init("Fractal", 1280, 720))
		demo.Start();
}
