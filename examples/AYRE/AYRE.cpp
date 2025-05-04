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
		quad = new Quad();
		basic = new Shader(
			"res/shaders/basic/basic.vs",
			"res/shaders/basic/ayre.fs"
		);
	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_ESCAPE))
			ShutDown();
	}

	void Simulate(f32 dt) override
	{
		acc_time += dt;
	}

	void Render() override
	{
		basic->Use();
		basic->SetUniform("acc_time", acc_time);
		basic->SetUniform("resolution", { 1280, 960 });
		quad->Draw();

		// GUI Code
		m_gui.m_func = [&]() {

		};
	}

private:
	Quad* quad = nullptr;
	Shader* shader = nullptr;
	Shader* basic = nullptr;

	f32 acc_time;
};

int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}