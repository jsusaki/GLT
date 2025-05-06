#include "Application.h"

class Minimal : public Application
{
public:
	Minimal() {}

	void Create() override
	{

	}

	void ProcessInput() override
	{

	}

	void Simulate(f32 dt) override
	{

	}

	void Render() override
	{

	}
};

int main()
{
	Minimal demo;
	if (demo.Init("Minimal", 800, 600))
		demo.Start();
}
