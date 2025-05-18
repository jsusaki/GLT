/*
	TODO: rewrite
*/
#pragma once

#include <vector>
#include "Core/Common.h"

// Grid Size
static const s32 N = 256;
// Grid Scale
static const s32 scale = 4;
// Index Function
s32 idx(s32 x, s32 y) { return y * N + x; }
// Forward declarations
static void set_bnd(s32 b, std::vector<f32>& x, s32 N);
static void lin_solve(s32 b, std::vector<f32>& x, std::vector<f32>& x0, f32 a, f32 c, s32 iter, s32 N);
static void diffuse(s32 b, std::vector<f32>& x, std::vector<f32>& x0, f32 diff, f32 dt, s32 iter, s32 N);
static void project(std::vector<f32>& velocX, std::vector<f32>& velocY, std::vector<f32>& p, std::vector<f32>& div, s32 iter, s32 N);
static void advect(s32 b, std::vector<f32>& d, std::vector<f32>& d0, std::vector<f32>& velocX, std::vector<f32>& velocY, f32 dt, s32 N);

// Fluid Model
struct FluidModel
{
	s32 size;
	f32 dt;
	f32 diff;
	f32 visc;
	std::vector<f32> s;
	std::vector<f32> density;

	std::vector<f32> Vx;
	std::vector<f32> Vy;
	std::vector<f32> Vx0;
	std::vector<f32> Vy0;

	FluidModel(s32 sz, s32 diffusion, s32 viscosity, s32 deltatime)
	{
		size = sz;
		dt = deltatime;
		diff = diffusion;
		visc = viscosity;

		s.resize(size * size);
		density.resize(size * size);

		Vx.resize(size * size);
		Vy.resize(size * size);
		Vx0.resize(size * size);
		Vy0.resize(size * size);
	}

	void AddDensity(s32 x, s32 y, f32 amount)
	{
		density[idx(x, y)] += amount;
	}

	void AddVelocity(s32 x, s32 y, f32 amountX, f32 amountY)
	{
		s32 index = idx(x, y);
		Vx[index] += amountX;
		Vy[index] += amountY;
	}

	void Step()
	{
		diffuse(1, Vx0, Vx, visc, dt, 4, N);
		diffuse(2, Vy0, Vy, visc, dt, 4, N);

		project(Vx0, Vy0, Vx, Vy, 4, N);

		advect(1, Vx, Vx0, Vx0, Vy0, dt, N);
		advect(2, Vy, Vy0, Vx0, Vy0, dt, N);

		project(Vx, Vy, Vx0, Vy0, 4, N);

		diffuse(0, s, density, diff, dt, 4, N);
		advect(0, density, s, Vx, Vy, dt, N);
	}
};


// Boundary Condition
static void set_bnd(s32 b, std::vector<f32>& x, s32 N)
{
	for (s32 i = 1; i < N - 1; i++)
	{
		x[idx(i, 0)] = b == 2 ? -x[idx(i, 1)] : x[idx(i, 1)];
		x[idx(i, N - 1)] = b == 2 ? -x[idx(i, N - 2)] : x[idx(i, N - 2)];
	}
	for (s32 j = 1; j < N - 1; j++)
	{
		x[idx(0, j)] = b == 1 ? -x[idx(1, j)] : x[idx(1, j)];
		x[idx(N - 1, j)] = b == 1 ? -x[idx(N - 2, j)] : x[idx(N - 2, j)];
	}

	x[idx(0, 0)] = 0.5 * (x[idx(1, 0)] + x[idx(0, 1)]);
	x[idx(0, N - 1)] = 0.5 * (x[idx(1, N - 1)] + x[idx(0, N - 2)]);
	x[idx(N - 1, 0)] = 0.5 * (x[idx(N - 2, 0)] + x[idx(N - 1, 1)]);
	x[idx(N - 1, N - 1)] = 0.5 * (x[idx(N - 2, N - 1)] + x[idx(N - 1, N - 2)]);
}

// Linear Equation Solver
static void lin_solve(s32 b, std::vector<f32>& x, std::vector<f32>& x0, f32 a, f32 c, s32 iter, s32 N)
{
	f32 cRecip = 1.0f / c;
	for (s32 k = 0; k < iter; k++)
	{
		for (s32 j = 1; j < N - 1; j++)
		{
			for (s32 i = 1; i < N - 1; i++)
			{
				x[idx(i, j)] = (x0[idx(i, j)] + a * (x[idx(i + 1, j)] + x[idx(i - 1, j)] + x[idx(i, j + 1)] + x[idx(i, j - 1)])) * cRecip;
			}
		}
		set_bnd(b, x, N);
	}
}

// Diffusion Equation
static void diffuse(s32 b, std::vector<f32>& x, std::vector<f32>& x0, f32 diff, f32 dt, s32 iter, s32 N)
{
	f32 a = dt * diff * (N - 2) * (N - 2);
	lin_solve(b, x, x0, a, 1 + 6 * a, iter, N);
}

// Projection Equation
static void project(std::vector<f32>& velocX, std::vector<f32>& velocY, std::vector<f32>& p, std::vector<f32>& div, s32 iter, s32 N)
{
	for (s32 j = 1; j < N - 1; j++) {
		for (s32 i = 1; i < N - 1; i++) {
			div[idx(i, j)] = (-0.5f * (velocX[idx(i + 1, j)] - velocX[idx(i - 1, j)] + velocY[idx(i, j + 1)] - velocY[idx(i, j - 1)])) / N;
			p[idx(i, j)] = 0;
		}
	}
	set_bnd(0, div, N);
	set_bnd(0, p, N);
	lin_solve(0, p, div, 1, 6, iter, N);

	for (s32 j = 1; j < N - 1; j++) {
		for (s32 i = 1; i < N - 1; i++) {
			velocX[idx(i, j)] -= 0.5f * (p[idx(i + 1, j)] - p[idx(i - 1, j)]) * N;
			velocY[idx(i, j)] -= 0.5f * (p[idx(i, j + 1)] - p[idx(i, j - 1)]) * N;
		}
	}
	set_bnd(1, velocX, N);
	set_bnd(2, velocY, N);
}

// Advection Equation
static void advect(s32 b, std::vector<f32>& d, std::vector<f32>& d0, std::vector<f32>& velocX, std::vector<f32>& velocY, f32 dt, s32 N)
{
	f32 i0, i1, j0, j1;

	f32 dtx = dt * (N - 2);
	f32 dty = dt * (N - 2);

	f32 s0, s1, t0, t1;
	f32 tmp1, tmp2, x, y;

	f32 Nfloat = N - 2;
	f32 ifloat, jfloat;
	s32 i, j;

	for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
		for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
			tmp1 = dtx * velocX[idx(i, j)];
			tmp2 = dty * velocY[idx(i, j)];
			x = ifloat - tmp1;
			y = jfloat - tmp2;

			if (x < 0.5f) x = 0.5f;
			if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
			i0 = std::floor(x);
			i1 = i0 + 1.0f;
			if (y < 0.5f) y = 0.5f;
			if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
			j0 = std::floor(y);
			j1 = j0 + 1.0f;

			s1 = x - i0;
			s0 = 1.0f - s1;
			t1 = y - j0;
			t0 = 1.0f - t1;

			s32 i0i = i0;
			s32 i1i = i1;
			s32 j0i = j0;
			s32 j1i = j1;

			d[idx(i, j)] = s0 * (t0 * d0[idx(i0i, j0i)] + t1 * d0[idx(i0i, j1i)]) + s1 * (t0 * d0[idx(i1i, j0i)] + t1 * d0[idx(i1i, j1i)]);
		}
	}
	set_bnd(b, d, N);
}

// 128 viridis color palette
static std::vector<Color> viridis = {
	{ 68, 1,   84 }, { 69,  4, 87 },  { 70,  7, 90  }, { 70, 10, 93 },
	{ 71, 13,  96 }, { 71, 16, 99 },  { 71, 19, 101 }, { 72, 22, 104 },
	{ 72, 24, 106 }, { 72, 27, 109 }, { 72, 29, 111 }, { 72, 32, 113 },
	{ 72, 35, 116 }, { 72, 37, 118 }, { 72, 40, 120 }, { 71, 42, 122 },
	{ 71, 45, 123 }, { 71, 47, 125 }, { 70, 50, 126 }, { 70, 52, 128 },
	{ 69, 55, 129 }, { 68, 57, 131 }, { 68, 59, 132 }, { 67, 62, 133 },
	{ 66, 64, 134 }, { 65, 66, 135 }, { 64, 69, 136 }, { 63, 71, 136 },
	{ 62, 73, 137 }, { 62, 76, 138 }, { 61, 78, 138 }, { 60, 80, 139 },
	{ 59, 82, 139 }, { 58, 84, 140 }, { 57, 86, 140 }, { 56, 89, 140 },
	{ 55, 91, 141 }, { 54, 93, 141 }, { 53, 95, 141 }, { 52, 97, 141 },
	{ 51, 99, 141 }, { 50, 101, 142 }, { 49, 103, 142 }, { 48, 105, 142 },
	{ 47, 107, 142 }, { 46, 109, 142 }, { 46, 111, 142 }, { 45, 113, 142 },
	{ 44, 114, 142 }, { 43, 116, 142 }, { 42, 118, 142 }, { 42, 120, 142 },
	{ 41, 122, 142 }, { 40, 124, 142 }, { 39, 126, 142 }, { 39, 128, 142 },
	{ 38, 130, 142 }, { 37, 131, 142 }, { 37, 133, 142 }, { 36, 135, 142 },
	{ 35, 137, 142 }, { 34, 139, 141 }, { 34, 141, 141 }, { 33, 143, 141 },
	{ 32, 146, 140 }, { 32, 147, 140 }, { 31, 149, 139 }, { 31, 151, 139 },
	{ 31, 153, 138 }, { 30, 155, 138 }, { 30, 157, 137 }, { 31, 159, 136 },
	{ 31, 161, 136 }, { 31, 162, 135 }, { 32, 164, 134 }, { 33, 166, 133 },
	{ 34, 168, 132 }, { 36, 170, 131 }, { 37, 172, 130 }, { 39, 173, 129 },
	{ 41, 175, 127 }, { 44, 177, 126 }, { 46, 179, 124 }, { 49, 181, 123 },
	{ 52, 182, 121 }, { 55, 184, 120 }, { 58, 186, 118 }, { 61, 188, 116 },
	{ 64, 189, 114 }, { 68, 191, 112 }, { 72, 193, 110 }, { 76, 194, 108 },
	{ 80, 196, 106 }, { 84, 197, 104 }, { 88, 199, 101 }, { 92, 200, 99 },
	{ 96, 202, 96 }, { 101, 203, 94 }, { 105, 205, 91 }, { 110, 206, 88 },
	{ 115, 208, 86 }, { 119, 209, 83 }, { 124, 210, 80 }, { 129, 211, 77 },
	{ 134, 213, 73 }, { 139, 214, 70 }, { 144, 215, 67 }, { 149, 216, 64 },
	{ 155, 217, 60 }, { 160, 218, 57 }, { 165, 219, 54 }, { 170, 220, 50 },
	{ 176, 221, 47 }, { 181, 222, 43 }, { 186, 222, 40 }, { 192, 223, 37 },
	{ 197, 224, 33 }, { 202, 225, 31 }, { 208, 225, 28 }, { 213, 226, 26 },
	{ 218, 227, 25 }, { 223, 227, 24 }, { 229, 228, 25 }, { 234, 229, 26 },
	{ 239, 229, 28 }, { 244, 230, 30 }, { 248, 230, 33 }, { 253, 231, 37 }
};

static Color GetColor(f32 val, f32 minVal, f32 maxVal)
{
	f32 v = std::min(std::max(val, minVal), maxVal);
	f32 d = maxVal - minVal;
	f32 normalized = (d == 0.0f) ? 0.5f : (v - minVal) / d;
	s32 index = static_cast<s32>(normalized * 127);
	return viridis[index];
}