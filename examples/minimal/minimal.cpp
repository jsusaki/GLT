#include "Application.h"

class Julia : public Application
{
public:
	Julia() {}

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
	Julia demo;
	if (demo.Init("Julia", 800, 600))
		demo.Start();
}
