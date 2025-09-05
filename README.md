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

## Examples

https://github.com/user-attachments/assets/8048eb98-d03d-4511-82bd-8f7fe80f1566

https://github.com/user-attachments/assets/f4f2d186-0f37-4c1d-8899-2e7c1c0e69fa

https://github.com/user-attachments/assets/c472c6c5-d5bd-4208-bbd9-2fc0ded2c2ed

https://github.com/user-attachments/assets/b876a06f-832f-4a9a-ad61-ca023e94d8ad



