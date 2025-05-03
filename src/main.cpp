#include "Application.h"

class Experiment : public Application
{
public:
	Experiment()
	{

	}

	void Create() override
	{

	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_SPACE))
			std::cout << "SPACE PRESSED!\n";
	}

	void Simulate(f32 dt) override
	{

	}

	void Render() override
	{

	}

private:

};


int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}