#pragma once

#include <algorithm>
#include <vector>

#include "Core/Common.h"

struct Color
{
    union { u32 c = 0xFFFFFF; struct { u8 r; u8 g; u8 b; u8 a; }; };

    inline constexpr Color() = default;
    inline constexpr Color(u8 r, u8 g, u8 b, u8 a = 0xFF) : r(r), g(g), b(b), a(a) {}
    inline constexpr Color(u32 hex, u8 a = 0xFF) : r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF), a(a) {}

    inline Color& operator =  (const Color& v) = default;
    inline bool   operator == (const Color& c) const { return this->c == c.c; }
    inline bool   operator != (const Color& c) const { return this->c != c.c; }
    inline Color  operator +  (const Color& c) const { u8 nr = r + c.r; u8 ng = g + c.g; u8 nb = b + c.b; return Color(nr, ng, nb, a); }
    inline Color  operator -  (const Color& c) const { u8 nr = r - c.r; u8 ng = g - c.g; u8 nb = b - c.b; return Color(nr, ng, nb, a); }
    inline Color  operator *  (const f32 i) const { u8 nr = u8(r * i); u8 ng = u8(g * i); u8 nb = u8(b * i); return Color(nr, ng, nb, a); }
    inline Color  operator *  (const Color& c) const { u8 nr = u8(r * c.r / 255.0f); u8 ng = u8(g * c.g / 255.0f); u8 nb = u8(b * c.b / 255.0f); u8 na = u8(a * c.a / 255.0f); return Color(nr, ng, nb, na); }
    inline Color  operator /  (const f32 i) const { u8 nr = u8(r / i); u8 ng = u8(g / i); u8 nb = u8(b / i); return Color(nr, ng, nb, a); }
    inline Color& operator += (const Color& c) { r += c.r; g += c.g; b += c.b; return *this; }
    inline Color& operator -= (const Color& c) { r -= c.r; g -= c.g; b -= c.b; return *this; }
    inline Color& operator *= (const f32 i) { r = u8(r * i); g = u8(g * i); b = u8(b * i); return *this; }
    inline Color& operator /= (const f32 i) { r = u8(r / i); g = u8(g / i); b = u8(b / i); return *this; }
    inline Color& operator *= (const Color& c) { r = u8(r * c.r / 255.0f); g = u8(g * c.g / 255.0f); b = u8(b * c.b / 255.0f); a = u8(a * c.a / 255.0f); return *this; }
    inline constexpr Color inv() const { u8 nr = 255 - r; u8 ng = 255 - g; u8 nb = 255 - b; return Color(nr, ng, nb, a); }
    inline constexpr u32 to_hex_rgb()  const { return (r << 16) | (g << 8) | b; }
    inline constexpr u32 to_hex_rgba() const { return (r << 24) | (g << 16) | (b << 8) | a; }
};

static inline Color lerp(Color a, Color b, f32 amount) { return a + (b - a) * amount; }
inline glm::vec4 to_float(const Color& c) { return glm::vec4(c.r, c.g, c.b, c.a) / 255.0f; }