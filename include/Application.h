#pragma once

#include <string>
#include <iostream>
#include <print>
#include <memory>
#include <chrono>
#include <algorithm>

#include "Core/Common.h"
#include "Core/Random.h"
#include "Core/Window.h"
#include "Core/Input.h"

class Application
{
public:
    Application();

public: // Interface
    bool Init(const std::string& title = "GL Template", s32 width = 800, s32 height = 600);
    bool Start();
    bool ShutDown();

protected: // Main functions to override
    virtual void Create();
    virtual void ProcessInput();
    virtual void Simulate(f32 dt);
    virtual void Render();

protected:
    Window m_window;
    Input m_input;

private:
    void UpdateFrameTime();

private:
    // Timing
    std::chrono::time_point<std::chrono::system_clock> m_t1;
    std::chrono::time_point<std::chrono::system_clock> m_t2;
    u32 m_last_fps;
    u32 m_frame_count;
    f32 m_frame_timer;
    f32 m_accumulator;
    f32 m_delta_time;
    f32 m_elapsed_time;
    f32 m_last_elapsed_time;
};
