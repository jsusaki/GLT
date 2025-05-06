#include "Application.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"

class Triangle : public Application
{
public:
	Triangle() {}

	std::unique_ptr<Shader> shader;
	std::unique_ptr<Mesh>   mesh;

public:
	void Create() override
	{
	    // Define Triangle
		std::vector<vertex> vertices = {
			// position               normal                color                       uv
			{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } }
		};
		std::vector<u32> indices = { 0, 1, 2 };
		mesh   = std::make_unique<Mesh>(vertices, indices);
		// Define Shader
		shader = std::make_unique<Shader>("res/shaders/basic/default.vs", "res/shaders/basic/default.fs");
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
		mesh->draw(GL_TRIANGLES);
	}
};

int main()
{
	Triangle demo;
	if (demo.Init("Triangle", 800, 600))
		demo.Start();
	return 0;
}