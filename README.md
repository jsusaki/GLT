# GLT (Graphic Library Template)

A template experiment framework for my projects.
Essentially, an input and GPU rendering wrapper with some graphic utilities.

```cpp

#include "Application.h"

class Experiment : public Application
{
public:
	Experiment() {}

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
};


int main()
{
	Experiment exp;
	if (exp.Init("Experiment", 1280, 960))
		exp.Start();
	return 0;
}
```