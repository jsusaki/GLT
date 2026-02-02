/*
		Audio Reactive Visualization: Waveforms
			1. Load .wav
			2. FFT
			3. Band Split
			4. 3D audio reactive waveform
*/
#include "Application.h"

#include "Graphics/Shader.h"
#include "Graphics/Sprite.h"

#include "Graphics/Mesh.h"
#include "Graphics/Camera.h"

#include "soloud.h"
#include "soloud_wav.h"

#include "dsp.h"

class AudioReactive : public Application
{
public:
	AudioReactive() {}

public:
	std::unique_ptr<Shader> prisma_shader;
	std::unique_ptr<Sprite> sprite;
	vf2 screen_size;
	f32 time = 0.0f;

	// Grid
	std::unique_ptr<Shader> grid_shader;
	std::unique_ptr<Grid> grid;

	// Camera
	Camera camera;
	vf2 prev_mouse;
	bool first_mouse = true;
	f32 move_speed = 2.5f;
	f32 mouse_sensitivity = 0.2f;
	f32 zoom_sensitivity = 0.1f;
	bool is_orthographic = false;

	//ArcballCamera camera;
	// Arcball
	vf2 mouse_pos_transformed = { 0.0f,0.0f };
	vf2 prev_mouse_pos_transformed = { 0.0f,0.0f };

	// SoLoud engine
	SoLoud::Soloud soloud;
	SoLoud::Wav wave;
	int music_handle;
	float* wav;
	float* fft;
	BandProcessor processor;

	vf3 movement;
	vf4 color = { 1.0f, 0.05f, 0.05f, 0.12f };

public:
	vf2 transform_mouse(vf2 p) { return vf2(p.x * 2.0f / m_window.Width() - 1.0f, 1.0f - 2.0f * p.y / m_window.Height()); }

	void Create() override
	{
		screen_size = { m_window.Width(), m_window.Height() };
		// Grid
		grid = std::make_unique<Grid>(256, 192, 0.25);
		grid_shader = std::make_unique<Shader>("res/shaders/audio_reactive/grid.vs", "res/shaders/audio_reactive/grid.fs");

		// Init soloud
		soloud.init(SoLoud::Soloud::ENABLE_VISUALIZATION);
		wave.load("res/audio/abyss.wav");

		// Camera
		//vf3 eye    = { 31.0f, 32.0f, 30.0f };
		//vf3 eye    = { 53.0f, 6.0f, 54.0f };
		vf3 eye    = { 0.0f, 1.2f, 54.0f };
		vf3 center = { 0.0f, 0.0f, 0.0f };
		vf3 up     = { 0.0f, 1.0f, 0.0f };
		f32 fov    = 65.0f;
		f32 near   = 0.1f;
		f32 far    = 100.0f;
		f32 aspect = static_cast<f32>(m_window.Width()) / m_window.Height();

		camera = Camera(eye, center, up, fov, aspect, near, far);


		/*
		// Arcball Camera
		vf3 eye = { 0.0f, 15.0f, 0.0f };
		vf3 center = { 0.0f,  0.0f, 0.0f };
		vf3 up = { -0.22f, 0.93f,-0.29f };
		f32 fov = 90.0f;
		f32 near = 0.1f;
		f32 far = 100.0f;
		f32 aspect = static_cast<f32>(m_window.Width()) / m_window.Height();
		camera = ArcballCamera(eye, center, up, fov, aspect, near, far);
		*/

	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_SPACE))
		{
			if (music_handle < 1)
				music_handle = soloud.playBackground(wave);
		}

		if (m_input.IsKeyPressed(GLFW_KEY_1))
		{
			soloud.setPause(music_handle, 0);
		}

		if (m_input.IsKeyPressed(GLFW_KEY_2))
			soloud.setPause(music_handle, 1);


		if (m_input.IsKeyPressed(GLFW_KEY_TAB))
		{
			is_orthographic = !is_orthographic;
			camera.set_orthographic(is_orthographic);
		}

		// Zoom
		f32 wheel_delta = m_input.GetMouseWheel();
		if (wheel_delta != 0.0f)
		{
			camera.zoom(wheel_delta * 0.1f);
		}

		prev_mouse_pos_transformed = mouse_pos_transformed;

		if (m_input.IsKeyHeld(GLFW_KEY_W)) movement += camera.front();
		if (m_input.IsKeyHeld(GLFW_KEY_S)) movement -= camera.front();
		if (m_input.IsKeyHeld(GLFW_KEY_D)) movement += camera.right();
		if (m_input.IsKeyHeld(GLFW_KEY_A)) movement -= camera.right();
		if (m_input.IsKeyHeld(GLFW_KEY_E)) movement += camera.up();
		if (m_input.IsKeyHeld(GLFW_KEY_Q)) movement -= camera.up();

		if (m_input.IsKeyHeld(GLFW_KEY_F)) m_gui.show_gui = !m_gui.show_gui;


		// Rotation
		if (m_input.IsButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
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

		/*
		// Arcball Rotation
		mouse_pos_transformed = transform_mouse(m_input.GetMouse());
		if (prev_mouse_pos_transformed != (vf2(-2.0f)))
		{
			if (m_input.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			{
				camera.rotate(prev_mouse_pos_transformed, mouse_pos_transformed);
			}
			if (m_input.IsButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			{
				vf2 dxy = mouse_pos_transformed - prev_mouse_pos_transformed;
				vf4 dxy4 = camera.inv_projection() * vf4(dxy.x, dxy.y, 0.0f, 1.0f);
				camera.pan(vf2(dxy4.x, dxy4.y));
			}
		}
		*/

		/*
		if (m_input.IsButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			vf2 curr_mouse = m_input.GetMouse();
			if (first_mouse)
			{
				prev_mouse = curr_mouse;
				first_mouse = false;
			}
			vf2 dxy = prev_mouse - curr_mouse;
			camera.pan(dxy);
		}
		else
		{
			first_mouse = true;
		}
		*/
	}

	void Simulate(f32 dt) override
	{
		time += dt;
		// Movement
		if (glm::length(movement) > 0.0f)
		{
			vf3 velocity = glm::normalize(movement) * move_speed;
			camera.translate(velocity * dt);
			movement = { 0.0f, 0.0f, 0.0f };
		}
	}

	void Render() override
	{
		m_window.Clear();

		// Audio Processing
		wav = soloud.getWave();
		fft = soloud.calcFFT();
		Bands raw    = extract_bands(fft);
		Bands motion = processor.update(raw);
		vf3 audio_uniform = { compress(motion.low()), compress(motion.medium()), compress(motion.treble()) };

		// Shader
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		grid_shader->Use();
		grid_shader->SetUniform("proj_view", camera.proj_camera());
		grid_shader->SetUniform("t", time);
		grid_shader->SetUniform("audio", audio_uniform);
		grid_shader->SetUniform("col", color);
		grid->draw(GL_LINES);

		m_gui.m_func = [&]() {
			ImGui::Begin("Output");
			ImGui::PlotLines("##Wave", wav, 256, 0, "Wave", -1, 1, ImVec2(264, 80));
			ImGui::PlotHistogram("##FFT", fft, 256 / 2, 0, "FFT", 0, 10, ImVec2(264, 80), 8);
			ImGui::Text("Audio (L/M/H): %.3f %.3f %.3f", audio_uniform.x, audio_uniform.y, audio_uniform.z);
			ImGui::Text("Eye:    x=%.3f y=%.3f z=%.3f", camera.eye().x, camera.eye().y, camera.eye().z);
			ImGui::Text("Up:     x=%.3f y=%.3f z=%.3f", camera.up().x, camera.up().y, camera.up().z);
			ImGui::Text("Pitch: %.1f  Yaw: %.1f", camera.pitch(), camera.yaw());
			ImGui::SliderFloat4("R=%.1f G=%.1f B=%.1f A=%.1f", glm::value_ptr(color), 0.0f, 1.0f);
			ImGui::End();
		};
	}

	void Destroy() override
	{
		soloud.deinit();
	}
};

int main()
{
	AudioReactive demo;
	if (demo.Init("Waveforms", 1024, 1024))
		demo.Start();
	return 0;
}