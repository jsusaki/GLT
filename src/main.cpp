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
		tex    = new Texture("res/images/wizardRL.png");
		quad   = new Quad();
		shader = new Shader("res/shaders/basic/basic.vs", "res/shaders/basic/basic.fs");
	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{

	}

	void Render() override
	{
		shader->Use();
		shader->SetUniform("screen_texture", 0);
		tex->Bind();
		quad->Draw();
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