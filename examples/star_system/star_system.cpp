/*
	2D Celestial Mechanics

	Kepler's law of planetary motion

	Orbit Mechanics

	Circular Motion
	Elliptical Motion

	Solar System
		Sun
		Earth
		Moon

	Drawing Routine
		Draw
		DrawLine
		DrawCircle
		DrawRing

	Antikythera mechanism

*/

#include "Application.h"

#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/Sprite.h"
#include "Graphics/Palette.h"

class StarSystem : public Application
{
public:
	StarSystem() {}

public:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Shader> texture_shader;
	s32 scale = 1;

	// TODO: Hierarchical Orbital System

	struct CelestialBody
	{
		vf2 pos = { 0.0f, 0.0f };
		vf2 vel = { 0.0f, 0.0f };
		f32 radius = 15.0f;
		f32 angle = 0.0f;
		f32 omega = 1.0f;
		f32 orbit_radius = 100.0f;
	};

	vf2 center;
	std::vector<CelestialBody> planets;
	bool simulate = false;

public:
	void Draw(s32 x, s32 y, s32 w, s32 h, Color c)
	{
		if (w == 1 && h == 1)
		{
			sprite->SetPixel(x, y, c.r, c.g, c.b, c.a);
			return;
		}

		for (s32 dx = 0; dx < w; dx++)
		{
			for (s32 dy = 0; dy < h; dy++)
			{
				sprite->SetPixel(x + dx, y + dy, c.r, c.g, c.b, c.a);
			}
		}
	}

	void DrawLine(s32 x0, s32 y0, s32 x1, s32 y1, Color c)
	{
		s32 dx = std::abs(x1 - x0);
		s32 dy = -std::abs(y1 - y0);
		s32 sx = (x0 < x1) ? 1 : -1;
		s32 sy = (y0 < y1) ? 1 : -1;
		s32 err = dx + dy;

		while (true)
		{
			Draw(x0, y0, scale, 1, c);

			if (x0 == x1 && y0 == y1) break;
			s32 e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; }
			if (e2 <= dx) { err += dx; y0 += sy; }
		}
	}

	void DrawRing(s32 cx, s32 cy, s32 outer_r, s32 inner_r, Color c)
	{
		s32 outer_r_sq = outer_r * outer_r;
		s32 inner_r_sq = inner_r * inner_r;

		s32 min_x = std::max(0, cx - outer_r);
		s32 max_x = std::min(m_window.Width() - 1, cx + outer_r);
		s32 min_y = std::max(0, cy - outer_r);
		s32 max_y = std::min(m_window.Height() - 1, cy + outer_r);

		for (s32 x = min_x; x <= max_x; x++)
		{
			for (s32 y = min_y; y <= max_y; y++)
			{
				s32 dx = x - cx;
				s32 dy = y - cy;
				s32 dist_sq = dx * dx + dy * dy;

				if (dist_sq >= inner_r_sq && dist_sq <= outer_r_sq)
				{
					Draw(x, y, scale, scale, c);
				}
			}
		}
	}

	void DrawCircle(s32 cx, s32 cy, s32 r, Color c)
	{
		s32 r_sq = r * r;

		s32 min_x = std::max(0, cx - r);
		s32 max_x = std::min(m_window.Width() - 1, cx + r);
		s32 min_y = std::max(0, cy - r);
		s32 max_y = std::min(m_window.Height() - 1, cy + r);

		for (s32 x = min_x; x <= max_x; x++)
		{
			for (s32 y = min_y; y <= max_y; y++)
			{
				s32 dx = x - cx;
				s32 dy = y - cy;
				s32 dist_sq = dx * dx + dy * dy;
				if (dist_sq <= r_sq)
				{
					Draw(x, y, scale, scale, c);
				}
			}
		}
	}

public:
	void Create() override
	{
		sprite = std::make_unique<Sprite>(m_window.Width(), m_window.Height());
		texture_shader = std::make_unique<Shader>("res/shaders/basic/texture.vs", "res/shaders/basic/texture.fs");
		s32 n = 9;

		center = { m_window.Width() / 2, m_window.Height() / 2 };

		CelestialBody p;
		p.pos = center;
		p.orbit_radius = 0.0f;
		p.omega = 0.0f;
		planets.push_back(p);

		for (s32 i = 0; i < n; i++)
		{
			CelestialBody p;
			p.orbit_radius = 35.0f * (i + 1.0f);
			p.omega = 0.5f + static_cast<f32>(i) * 0.1f;
			p.pos = {
				static_cast<f32>(center.x + p.orbit_radius + (i * 5)) ,
				static_cast<f32>(center.y + p.orbit_radius + (i * 5)) ,
			};
			planets.push_back(p);
		}

	}

	void ProcessInput() override
	{
		if (m_input.IsKeyPressed(GLFW_KEY_SPACE))
			simulate = !simulate;
	}

	void Simulate(f32 dt) override
	{
		if (simulate)
		{
			for (auto& p : planets)
			{
				p.angle += p.omega * dt;
				p.pos.x = center.x + p.orbit_radius * std::cos(p.angle);
				p.pos.y = center.y + p.orbit_radius * std::sin(p.angle);
			}
		}
	}

	void Render() override
	{
		m_window.Clear();
		sprite->Clear();

		texture_shader->Use();
		texture_shader->SetUniform("screen_texture", 0);

		for (size_t i = 0; i < planets.size(); i++)
		{
			auto& p = planets[i];
			DrawCircle(p.pos.x, p.pos.y, p.radius, viridis_16[i % 16]);
			DrawRing(center.x, center.y, p.orbit_radius, p.orbit_radius - 1.0f, { 128, 128, 0 });
			DrawLine(center.x, center.y, p.pos.x, p.pos.y, { 128, 128, 128 });

			if (i < planets.size() - 1)
			{
				const auto& next_p = planets[i + 1];
				DrawLine(p.pos.x, p.pos.y, next_p.pos.x, next_p.pos.y, { 255, 255, 0 });
			}
		}

		sprite->UpdateTexture();
		sprite->Draw();
	}
};

int main()
{
	StarSystem demo;
	if (demo.Init("StarSystem", 1280, 720))
		demo.Start();
}
