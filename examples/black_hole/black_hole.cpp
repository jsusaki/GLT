/*
	Black Hole Simulation

		Method: 
			Raytracing + Black Hole Physics

		Concepts:
			Solar Mass:
				SM = 1.989 * 10^30 kg
			Mass:    
				M = 2.6 * 10^9 * SM
			Gravity: 
				G = 6.67 * 10^-11 N
			Speed of Light: 
				c = 299 792 458 m/s
			Event Horizon (Schwarzchild metric?)
				Rs = 2 * G * M / c^2
			Accretion Disk:
				3*Rs
			Photon Sphere:
				1.5*Rs

		Referenes:
			https://github.com/kavan010/black_hole
			https://github.com/rossning92/Blackhole
			https://oseiskar.github.io/black-hole/docs/physics.html
			https://eliot1019.github.io/Black-Hole-Raytracer/
			https://rantonels.github.io/starless/
*/


#include "Application.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"

#include "Graphics/Camera.h"

class Triangle : public Application
{
public:
	Triangle() {}

	std::unique_ptr<Shader> shader;
	std::unique_ptr<Mesh> mesh;

	PerspectiveCamera camera;

	// control state
	vf2 last_mouse;
	bool first_mouse = true;

	f32 move_speed = 2.5f;
	f32 mouse_sens = 0.2f;
	f32 zoom_sens = 0.1f;

public:
	void Create() override
	{
		// Define Triangle
		std::vector<vertex> vertices = {
			// position               normal                color                       uv
			{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } }
		};
		std::vector<u32> indices = { 0, 1, 2 };
		mesh = std::make_unique<Mesh>(vertices, indices);

		shader = std::make_unique<Shader>("res/shaders/basic/default.vs", "res/shaders/basic/default.fs");

		// Create Camera
		vf3 eye    = { 0.0f, 0.0f, 3.0f };
		vf3 center = { 0.0f, 0.0f, 0.0f };
		vf3 up     = { 0.0f, 1.0f, 0.0f };
		f32 fov    = 60.0f;
		f32 near   = 0.1f;
		f32 far   = 100.0f;
		f32 aspect = static_cast<f32>(m_window.Width()) / m_window.Height();

		camera = PerspectiveCamera(eye, center, up, fov, aspect, near, far);
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{
		// Movement
		if (m_input.IsKeyHeld(GLFW_KEY_W)) camera.translate(camera.front()  * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_S)) camera.translate(-camera.front() * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_A)) camera.translate(-camera.right() * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_D)) camera.translate(camera.right()  * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_Q)) camera.translate(-camera.up()    * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_E)) camera.translate(camera.up()     * move_speed * dt);

		// Rotation
		if (m_input.IsButtonPressed(0))
		{
			vf2 curr_mouse = m_input.GetMouse();
			if (first_mouse)
			{
				last_mouse = curr_mouse;
				first_mouse = false;
			}

			camera.rotate(last_mouse, curr_mouse);
			last_mouse = curr_mouse;
		}
		else
		{
			first_mouse = true;
		}

		// Zoom
		f32 wheel_delta = m_input.GetMouseWheel();
		if (wheel_delta != 0.0f)
		{
			camera.zoom(wheel_delta * zoom_sens);
		}
	}

	void Render() override
	{
		m_window.Clear();
		shader->Use();

		mf4x4 model = mf4x4(1.0f); // identity
		mf4x4 model_view_projection = camera.projection() * camera.view() * model;
		shader->SetUniform("proj_view", model_view_projection);

		//shader->SetUniform("proj_view", camera.proj_camera());
		mesh->draw(GL_TRIANGLES);
		shader->Unuse();
	}
};

int main()
{
	Triangle demo;
	if (demo.Init("Triangle", 1280, 720))
		demo.Start();
	return 0;
}