#include "Application.h"

class Minimal : public Application
{
public:
	Minimal() {}

public:
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

private:

};

int main()
{
	Minimal app;
	if (app.Init("Minimal", 800, 600))
		app.Start();
	return 0;
}