#include "Application.h"

#include "Graphics/Texture.h"
#include "Graphics/Quad.h"
#include "Graphics/Shader.h"
#include "Graphics/PostProcessor.h"

class Experiment : public Application
{
public:
	Experiment() {}

	Quad* quad = nullptr;
	Texture* tex = nullptr;
	Shader* shader = nullptr;
	Shader* basic = nullptr;
	PostProcessor* post_processor = nullptr;

public:
	void Create() override
	{
		// Load and Initialize Resources
		tex = new Texture("res/images/wizardRL.png");
		quad = new Quad();
		basic = new Shader(
			"res/shaders/basic/basic.vs",
			"res/shaders/basic/basic.fs"
		);

		shader = new Shader(
			"res/shaders/post_processing/post_processing.vs",
			"res/shaders/post_processing/grayscale.fs"
		);

		post_processor = new PostProcessor(m_window.Width(), m_window.Height());
	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_ESCAPE))
			ShutDown();
	}

	void Simulate(f32 dt) override
	{
		// Update logic
	}

	void Render() override
	{
		post_processor->Begin();

		basic->Use();
		basic->SetUniform("screen_texture", 0);
		tex->Bind();
		quad->Draw();

		shader->Use();
		shader->SetUniform("screen_texture", 0);

		m_gui.m_func = [&]() {
			// GUI Code
			};

		post_processor->End();
		post_processor->Render();
	}

};

/*
int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}
*/