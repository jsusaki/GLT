/*
	Armored Core 6 - Coral Contact: Ayre

	https://youtu.be/EgqUkCTS-Yc?t=116

	https://armoredcore.fandom.com/wiki/Ayre

	Scene sequence description
		1. black background
		2. small center red dot intensifying slowly
		2. small center red dot splash sparking network
		3. small to medium center red dot with glint sparking
		4. large center red dot with glint unfold into a network

	- random number generation techniques
	- noise techniques (seed)
	- network propagation techniques (growing)
	- interpolation techniques
	- glint techniques
	- color gradient techniques

	Particle System
	Perlin Noise
	Flow Field
*/

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
		quad  = new Quad();
		basic = new Shader(
			"res/shaders/basic/basic.vs",
			"res/shaders/basic/ayre.fs"
		);

		

	}

	void ProcessInput() override
	{

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
	Shader* basic = nullptr;

	f32 acc_time;


	struct Particle
	{
		vf2 pos;
		vf2 vel;
	};

	std::vector<Particle> particles;
};

int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}