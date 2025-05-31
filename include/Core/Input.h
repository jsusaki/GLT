#pragma once

#include <array>

#include "Common.h"

class Input
{
public:
    Input() {}

    // Call this once per frame
    inline void Update() { prev_keys = keys; prev_mouse_pos = mouse_pos; ResetMouseWheel(); }

    // Key Interface
    inline bool IsKeyPressed(const s32 key)  const { return keys[key] && !prev_keys[key]; }
    inline bool IsKeyHeld(const s32 key)     const { return keys[key]; }
    inline bool IsKeyReleased(const s32 key) const { return !keys[key] && prev_keys[key]; }

    // Mouse Interface
    inline vf2 GetMouse() { return mouse_pos; }
    inline vf2 GetMouseDelta() { return prev_mouse_pos - mouse_pos; }
    inline bool IsButtonPressed(const s32 button) const { return buttons[button]; }
    inline f32 GetMouseWheel() { return mouse_wheel_delta; }
    inline void ResetMouseWheel() { mouse_wheel_delta = 0; }


public:
    // Internal, used by GLFW
    inline void SetKey(const s32 key, const bool pressed) { prev_keys[key] = keys[key]; keys[key] = pressed; }
    inline void SetButton(const s32 button, const bool pressed) { buttons[button] = pressed; }
    inline void SetMousePos(const f64 x, const f64 y) { prev_mouse_pos = mouse_pos; mouse_pos = { static_cast<f32>(x), static_cast<f32>(y) }; }
    inline void SetMouseWheelDelta(const f32 delta) { mouse_wheel_delta += delta; }

private:
    static const s32 MAX_KEYS = GLFW_KEY_LAST;
    static const s32 MAX_BUTTONS = 5;

    std::array<bool, MAX_KEYS>    keys = {};
    std::array<bool, MAX_KEYS>    prev_keys = {};
    std::array<bool, MAX_BUTTONS> buttons = {};

    vf2 mouse_pos = {};
    vf2 prev_mouse_pos = {};
    f32 mouse_wheel_delta = 0.0f;
};