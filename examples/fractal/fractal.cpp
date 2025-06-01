/*
	Fractal Explorer
		Mandelbrot Shader
		Julia Shader

	https://paulbourke.net/fractals/juliaset/
	https://www.karlsims.com/julia.html
	https://acko.net/blog/how-to-fold-a-julia-fractal/
*/
#include "Application.h"

#include "Graphics/Sprite.h"
#include "Graphics/Shader.h"

#include "Core/Random.h"

class Fractal : public Application
{
public:
	Fractal() {}

public:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> fractal_shader;
	bool show_julia = true;

	// Beautiful julia constants
	vf2 c = { -0.7269f, 0.1889f };
	//vf2 c = {-0.943072290, 0.258960843};
	//vf2 c = {-0.772466100, 0.103424500};
	//vf2 c = {-0.743510000, 0.127090000};
	//vf2 c = {-0.79, 0.15};

	f32 zoom = 0.9f;
	s32 max_iter = 85;

	f32 pan_sensitivity = 0.003f;
	f32 zoom_sensitivity = 0.1f;
	vf2 center_offset = { 0.0f, 0.0f };
	vf2 screen_size;

	bool animate_julia = false;
	bool ping_pong = false;
	f32 animation_speed = 0.1f;
	f32 angle = 0.0f;
	f32 timer = 0.0f;
	f32 ping_pong_interval = 5.0f;

	void Create() override
	{
		screen_size    = { m_window.Width(), m_window.Height() };
		sprite         = std::make_unique<Sprite>(screen_size.x, screen_size.y);
		fractal_shader = std::make_unique<Shader>("res/shaders/basic/fractal.vs", "res/shaders/basic/fractal.fs");
	}

	void ProcessInput() override
	{
		// Show GUI
		if (m_input.IsKeyPressed(GLFW_KEY_TAB))
			m_gui.show_gui = !m_gui.show_gui;

		// Reset
		if (m_input.IsKeyPressed(GLFW_KEY_R))
			zoom = 0.9f;

		// Animate
		if (m_input.IsKeyPressed(GLFW_KEY_SPACE))
			animate_julia = !animate_julia;

		// Pan
		if (m_input.IsButtonPressed(1))
		{
			vf2 delta = m_input.GetMouseDelta();
			center_offset.x -= delta.x * -pan_sensitivity * zoom;
			center_offset.y += delta.y * -pan_sensitivity * zoom;
		}

		// Zoom
		f32 wheel_delta = m_input.GetMouseWheel();
		if (wheel_delta != 0.0f)
		{
			f32 aspect_ratio = screen_size.x / screen_size.y;
			vf2 mouse_position = m_input.GetMouse();
			vf2 mouse_ndc = (mouse_position / screen_size) * 2.0f - 1.0f;
			mouse_ndc.x *= aspect_ratio;
			mouse_ndc.y = -mouse_ndc.y;

			vf2 before_zoom = mouse_ndc * zoom + center_offset;

			f32 zoom_factor = std::exp(-wheel_delta * zoom_sensitivity);
			zoom = std::clamp(zoom * zoom_factor, 1e-6f, 1.5f);

			vf2 after_zoom = mouse_ndc * zoom + center_offset;

			center_offset += before_zoom - after_zoom;
		}
	}
	
	void Simulate(f32 dt) override
	{
		timer += dt;
		f32 dir = 1.0f;
		if (animate_julia) 
		{
			// Ping Pong Animation
			if (ping_pong)
			{
				f32 t = fmodf(timer, ping_pong_interval * 2.0f) / ping_pong_interval;
				dir = t < 1.0f ? 1.0f : -1.0f;
			}

			// Update Angle
			angle += dt * animation_speed * dir; 

			// Update C
			c = {
				std::cos(angle) + std::sin(angle * 3.0f) * 0.5f, 
				std::sin(angle) 
			};
			c *= 0.75f;

			//c = { std::cos(angle), std::sin(angle) };
		}
	}

	void Render() override
	{
		m_window.Clear();

		fractal_shader->Use();
		fractal_shader->SetUniform("screen_size", screen_size);
		fractal_shader->SetUniform("zoom", zoom);
		fractal_shader->SetUniform("max_iter", max_iter);
		fractal_shader->SetUniform("c", c);
		fractal_shader->SetUniform("show_julia", show_julia);
		fractal_shader->SetUniform("center_offset", center_offset);

		sprite->Draw();

		m_gui.m_func = [&]() {
			ImGui::Begin("Fractal Parameters");

			ImGui::DragFloat("Zoom", &zoom, 0.01f, 1e-6f, 1.0f);
			ImGui::DragInt("Max Iteration", &max_iter, 1, 1, 5000);
			ImGui::DragFloat2("c", glm::value_ptr(c), 0.001f, -1.0f, 1.0f);

			if (ImGui::Button(show_julia ? "Mandelbrot Set" : "Julia Set"))
				show_julia = !show_julia;

			ImGui::BeginDisabled(!show_julia);
			ImGui::Checkbox("Animate", &animate_julia);
			ImGui::DragFloat("Animation Speed", &animation_speed, 0.01f, -2.0f, 2.0f);
			ImGui::Checkbox("Ping Pong", &ping_pong);
			ImGui::DragFloat("Ping Pong Interval", &ping_pong_interval, 0.01f, 0.0f, 10.0f);
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
