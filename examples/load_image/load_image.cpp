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
		// Load and Initialize Resources
		tex    = new Texture("res/images/wizardRL.png");
		quad   = new Quad();
		shader = new Shader(
			"res/shaders/post_processing/post_processing.vs", 
			"res/shaders/post_processing/grayscale.fs"
		);
	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_ESCAPE))
			ShutDown();
	}

	void Simulate(f32 dt) override
	{
		// Update simulation logic
	}

	void Render() override
	{
		shader->Use();
		shader->SetUniform("screen_texture", 0);

		tex->Bind();
		quad->Draw();

		m_gui.m_func = [&]() {
			// GUI Code
		};
	}

private:
	Quad* quad     = nullptr;
	Texture* tex   = nullptr;
	Shader* shader = nullptr;
};

int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}