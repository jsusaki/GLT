#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <glad/glad.h>

#include "Core/Common.h"
#include "Color.h"
#include "Shader.h"

struct vertex
{
	vf3 position;
	vf3 normal;
	vf4 color;
	vf2 uv;
};

struct Mesh
{
	u32 vao, vbo, ibo;
	std::vector<vertex> vertices;
	std::vector<u32> indices;

	Mesh() {};

	Mesh(const std::string& filepath)
	{
		load_from_file(filepath);
		setup_buffers();
	}

	Mesh(std::vector<vertex> v, std::vector<u32> i)
	{
		vertices = v; indices = i;
		setup_buffers();
	}

	~Mesh()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	void draw(s32 mode)
	{
		glBindVertexArray(vao);
		glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void setup_buffers()
	{
		// Generate vertex buffers
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
		// Bind vertex array object
		glBindVertexArray(vao);

		// Vertex Buffer Object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		// Index Buffer Object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);

		// Vertex Attribute configuration
		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));

		// Normal attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));

		// Color attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));

		// Texture coordinate attribute
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void load_from_file(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
			std::cout << "ERROR: File does not exist\n";

		// buffers
		std::vector<vf3> vertex_points;
		std::vector<vf3> vertex_normals;
		std::vector<vf2> vertex_texcoords;

		std::vector<vi3> vertex_indices;
		std::vector<vi2> vertex_texture_indices;
		std::vector<vi3> vertex_normal_indices;

		std::string prefix;
		while (file >> prefix)
		{
			if (prefix == "v") // Point
			{
				vf3 p;
				file >> p.x >> p.y >> p.z;
				vertex_points.push_back(p);
			}
			else if (prefix == "vt") // Texture
			{
				vf2 t;
				file >> t.x >> t.y;
				vertex_texcoords.push_back(t);
			}
			else if (prefix == "vn") // Normal
			{
				vf3 n;
				file >> n.x >> n.y >> n.z;
				vertex_normals.push_back(n);
			}
			else if (prefix == "f") // Face
			{
				// Buffers
				u8 slash;
				vi3 v_idx, v_tex_idx, v_norm_idx;
				for (s32 i = 0; i < 3; i++)
					file >> v_idx[i] >> slash >> v_tex_idx[i] >> slash >> v_norm_idx[i];

				// Obj idx starts from 1, so subtract 1
				vertex_indices.push_back(v_idx - 1);
				vertex_texture_indices.push_back(v_tex_idx - 1);
				vertex_normal_indices.push_back(v_norm_idx - 1);
			}
		}

		file.close();

		for (s32 i = 0; i < vertex_indices.size(); i++)
		{
			for (s32 j = 0; j < 3; j++)
			{
				vertex v;
				v.position = vertex_points[vertex_indices[i][j]];
				v.normal   = vertex_normals[vertex_normal_indices[i][j]];
				v.color    = to_float({ 255, 255, 255, 255 });
				v.uv       = vertex_texcoords[vertex_texture_indices[i][j]];
				vertices.push_back(v);
				indices.push_back(vertices.size() - 1);
			}
		}
	}
};

struct Prisma : public Mesh
{
	Prisma()
	{
		vertices = {
			// position               normal                color                       uv
			{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } }
		};
		indices = { 0, 1, 2 };
		setup_buffers();
	}
};

struct Quad : public Mesh
{
	Quad()
	{
		vertices = {
			//    position           normal            color (white)			// uv
			{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f }}, // bottom-left
			{{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f }}, // bottom-right
			{{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f }}, // top-right
			{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f }}, // top-left
		};

		indices = {
			0, 1, 2,
			2, 3, 0
		};

		setup_buffers();
	}
};

struct Grid : public Mesh
{
	Grid(int w, int h, float spacing = 1.0f)
	{
		vertices.clear();
		indices.clear();

		float total_width = (w - 1) * spacing;
		float total_height = (h - 1) * spacing;

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				float u = (float)x / (w - 1);
				float v = (float)y / (h - 1);

				float px = (x * spacing) - (total_width  * 0.5f);
				float py = (y * spacing) - (total_height * 0.5f);

				vertices.push_back({
					{ px, py, 0.0f },
					{ 0.0f, 1.0f, 0.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f},
					{ u, v }
				});
			}
		}

		for (int y = 0; y < h - 1; ++y)
		{
			for (int x = 0; x < w - 1; ++x)
			{
				int i = y * w + x;
				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + w);
				indices.push_back(i + 1);
				indices.push_back(i + w + 1);
				indices.push_back(i + w);
			}
		}
		setup_buffers();
	}
};


struct Cube : public Mesh
{
	Cube()
	{
		vertices = {
												     // color: white
			{{-1.0f, -1.0f,-1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f }}, //v0
			{{ 1.0f, -1.0f,-1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f }}, //v1
			{{ 1.0f,  1.0f,-1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f }}, //v2
			{{-1.0f,  1.0f,-1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f }}, //v3

			{{-1.0f, -1.0f, 1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 1.0f, 0.0f }}, //v4
			{{ 1.0f, -1.0f, 1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f }}, //v5
			{{ 1.0f,  1.0f, 1.0f},{1.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 1.0f, 1.0f }}, //v6
			{{-1.0f,  1.0f, 1.0f},{0.0f, 0.0f,-1.0f},{1.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 1.0f }}, //v7
		};

		indices = { 
			0,1,  // v0-v1
			1,2,  // v1-v2
			2,3,  // v2-v3
			3,0,  // v3-v0
			4,5,  // v4-v5
			5,6,  // v5-v6
			6,7,  // v6-v7
			7,4,  // v7-v4
			0,4,  // v0-v4
			1,5,  // v1-v5
			2,6,  // v2-v6
			3,7   // v3-v7
		};

		setup_buffers();
	}
};

struct Sphere : public Mesh
{
	Sphere(f32 radius = 1.0f, u32 sector = 36, u32 stack = 18)
	{
		// Generate vertices
		for (u32 i = 0; i <= stack; i++)
		{
			f32 angle = PI / 2 - i * PI / stack;     // from pi/2 to -pi/2
			f32 xy = radius * std::cosf(angle);      // r * cos(u)
			f32 z  = radius * std::sinf(angle);      // r * sin(u)

			for (u32 j = 0; j <= sector; j++)
			{
				f32 sectorAngle = j * 2 * PI / sector; // 0 to 2pi

				f32 x = xy * std::cosf(sectorAngle);
				f32 y = xy * std::sinf(sectorAngle);

				glm::vec3 position = { x, y, z };
				glm::vec3 normal   = glm::normalize(position);
				glm::vec4 color    = { 1.0f, 1.0f, 1.0f, 1.0f };
				glm::vec2 uv       = { (f32)j / sector, (f32)i / stack };

				vertices.push_back({ position, normal, color, uv });
			}
		}

		// Generate indices
		for (u32 i = 0; i < stack; i++)
		{
			u32 k1 = i * (sector + 1); // beginning of current stack
			u32 k2 = k1 + sector + 1;  // beginning of next stack

			for (u32 j = 0; j < sector; j++, k1++, k2++)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (stack - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		setup_buffers();
	}
};

struct Ring : public Mesh
{
	Ring(float innerRadius = 0.5f, float outerRadius = 1.0f, unsigned int segments = 64)
	{
		vertices.clear();
		indices.clear();

		float step = glm::two_pi<float>() / static_cast<float>(segments);

		for (unsigned int i = 0; i <= segments; ++i)
		{
			float theta = i * step;
			float cosT = cos(theta);
			float sinT = sin(theta);

			// outer vertex
			vf3 posOuter = { outerRadius * cosT, 0.0f, outerRadius * sinT };
			vf3 normal = { 0.0f, 0.0f, 1.0f };
			vf4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
			vf2 uvOuter = { (cosT + 1.0f) * 0.5f, (sinT + 1.0f) * 0.5f };
			vertices.push_back({ posOuter, normal, color, uvOuter });

			// inner vertex
			vf3 posInner = { innerRadius * cosT, 0.0f, innerRadius * sinT };
			glm::vec2 uvInner = {
				(cosT * (innerRadius / outerRadius) + 1.0f) * 0.5f,
				(sinT * (innerRadius / outerRadius) + 1.0f) * 0.5f 
			};
			vertices.push_back({ posInner, normal, color, uvInner });
		}

		// generate triangle indices
		for (unsigned int i = 0; i < segments; ++i)
		{
			unsigned int outer0 = i * 2;
			unsigned int inner0 = i * 2 + 1;
			unsigned int outer1 = (i + 1) * 2;
			unsigned int inner1 = (i + 1) * 2 + 1;

			indices.push_back(outer0);
			indices.push_back(inner0);
			indices.push_back(outer1);

			indices.push_back(outer1);
			indices.push_back(inner0);
			indices.push_back(inner1);
		}

		setup_buffers();
	}
};



struct Model
{
	mf4x4 m_model;
	Mesh m_mesh;
	vf3 m_position;
	vf3 m_rotation;
	vf3 m_scale;
	f32 m_angle;

	Model(const std::string& filepath)
	{
		m_model    = mf4x4(1.0f);
		m_position = vf3(0.0f, 0.0f, 0.0f);
		m_rotation = vf3(0.0f, 0.0f, 0.0f);
		m_scale    = vf3(1.0f, 1.0f, 1.0f);
		m_angle    = 0.0f;
		m_mesh     = Mesh(filepath);
	}

	Model(Mesh& m)
	{
		m_model    = mf4x4(1.0f);
		m_position = vf3(0.0f, 0.0f, 0.0f);
		m_rotation = vf3(0.0f, 0.0f, 0.0f);
		m_scale    = vf3(1.0f, 1.0f, 1.0f);
		m_angle    = 0.0f;
		m_mesh     = m;
	}

	void draw(std::shared_ptr<Shader> prisma_shader, s32 mode = GL_TRIANGLES)
	{
		prisma_shader->Use();

		m_model = mf4x4(1.0f);
		m_model = glm::rotate(m_model, glm::radians(m_angle), { 1.0f, 0.0f, 0.0f });
		m_model = glm::rotate(m_model, glm::radians(m_angle), { 0.0f, 1.0f, 0.0f });
		m_model = glm::rotate(m_model, glm::radians(m_angle), { 0.0f, 0.0f, 1.0f });

		m_model = glm::translate(m_model, m_position);
		m_model = glm::scale(m_model, m_scale);

		prisma_shader->SetUniform("model", m_model);
		m_mesh.draw(mode);
		prisma_shader->Unuse();
	}

	void scale(vf3 scale) { m_scale *= scale; }
	void rotate(f32 angle, vf3 axis) { m_rotation += axis; m_angle += angle; }
	void translate(vf3 pos) { m_position = pos; }
};