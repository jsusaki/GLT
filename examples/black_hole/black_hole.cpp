/*
	Black Hole Simulation

		Method: 
			Raytracing + Black Hole Physics

		Concepts:
			Solar Mass:     SM = 1.989 * 10^30 kg
			Mass:            M = 2.6 * 10^9 * SM
			Gravity:         G = 6.67 * 10^-11 N
			Speed of Light:  c = 299 792 458 m/s
			Event Horizon (Schwarzchild radius)
				Rs = 2 * G * M / c^2
			Accretion Disc: 3.0 * Rs
			Photon Sphere:  1.5 * Rs

		Graphics:
			Gravitational Lensing


		Scene
			Celestial Sphere (Skybox)
			Black Hole (Sphere)
				Accretion Disc (Ring)
				Photon Sphere (Sphere)
			Camera (Ray Tracing)

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

	std::unique_ptr<Sphere> sphere;
	std::unique_ptr<Ring> ring;

	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;


	PerspectiveCamera camera;

	// control state
	vf2 prev_mouse;
	bool first_mouse = true;

	f32 move_speed = 2.5f;
	f32 mouse_sensitivity = 0.2f;
	f32 zoom_sensitivity  = 0.1f;

public:
	void Create() override
	{
		// Background Stars
		sprite = std::make_unique<Sprite>("res/images/nasa.png");
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");

		// Black Hole
		sphere = std::make_unique<Sphere>();
		ring   = std::make_unique<Ring>(2.0f, 3.0f);
		black_hole_shader = std::make_unique<Shader>("res/shaders/basic/default.vs", "res/shaders/black_hole/black_hole.fs");

		// Camera
		vf3 eye    = { 0.0f, 0.0f, 3.0f };
		vf3 center = { 0.0f, 0.0f, 0.0f };
		vf3 up     = { 0.0f, 1.0f, 0.0f };
		f32 fov    = 60.0f;
		f32 near   = 0.1f;
		f32 far    = 100.0f;
		f32 aspect = static_cast<f32>(m_window.Width()) / m_window.Height();

		camera = PerspectiveCamera(eye, center, up, fov, aspect, near, far);
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{
		// Movement
		if (m_input.IsKeyHeld(GLFW_KEY_W)) camera.translate( camera.front() * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_S)) camera.translate(-camera.front() * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_D)) camera.translate( camera.right() * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_A)) camera.translate(-camera.right() * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_E)) camera.translate( camera.up()    * move_speed * dt);
		if (m_input.IsKeyHeld(GLFW_KEY_Q)) camera.translate(-camera.up()    * move_speed * dt);

		// Rotation
		if (m_input.IsButtonPressed(0))
		{
			vf2 curr_mouse = m_input.GetMouse();
			if (first_mouse)
			{
				prev_mouse = curr_mouse;
				first_mouse = false;
			}

			camera.rotate(prev_mouse, curr_mouse);
			prev_mouse = curr_mouse;
		}
		else
		{
			first_mouse = true;
		}

		// Zoom
		f32 wheel_delta = m_input.GetMouseWheel();
		if (wheel_delta != 0.0f)
		{
			camera.zoom(wheel_delta * zoom_sensitivity);
		}
	}

	void Render() override
	{
		m_window.Clear();

		// Background Stars
		//texture_shader->Use();
		//texture_shader->SetUniform("screen_texture", 0);
		//sprite->Draw();
		//texture_shader->Unuse();

		black_hole_shader->Use();
		mf4x4 model = mf4x4(1.0f); // identity
		mf4x4 model_view_projection = camera.projection() * camera.view() * model;
		black_hole_shader->SetUniform("proj_view", model_view_projection);
		//shader->SetUniform("proj_view", camera.proj_camera());

		sphere->draw(GL_TRIANGLES);
		ring->draw(GL_TRIANGLES);
		black_hole_shader->Unuse();

	}
};

int main()
{
	BlackHole demo;
	if (demo.Init("BlackHole", 1280, 720))
		demo.Start();
	return 0;
}