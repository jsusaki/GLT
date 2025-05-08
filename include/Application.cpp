#include "Application.h"

Application::Application()
{

}

bool Application::Init(const std::string& title, s32 width, s32 height)
{
    // Window
    m_window.Init(title, width, height);
    m_window.SetInput(&m_input);

    // GUI
    m_gui.Init(m_window.GetWindow());

    // Time
    m_t1 = std::chrono::system_clock::now();
    m_t2 = std::chrono::system_clock::now();
    m_last_fps = 0;
    m_frame_timer = 1.0f;
    m_frame_count = 0;
    m_accumulator = 0.0f;
    m_delta_time = 1.0f / 60.0f;
    m_elapsed_time = 0.0f;
    m_last_elapsed_time = 0.0f;

    // Create User Application Resources
    Create();

    return true;
}

bool Application::Start()
{
    while (!m_window.ShouldClose())
    {
        // Poll events
        m_window.PollEvents();

        // Handle timing
        m_t2 = std::chrono::system_clock::now();
        std::chrono::duration<f32> elapsed_time = m_t2 - m_t1;
        m_t1 = m_t2;

        // Compute elapsed time
        m_elapsed_time = elapsed_time.count();
        m_last_elapsed_time = m_elapsed_time;

        // Handle User Input
        ProcessInput();

        // Update input state
        m_input.Update();

        // Fixed Time Update
        m_accumulator += m_delta_time;
        while (m_accumulator >= m_delta_time)
        {
            Simulate(m_elapsed_time);
            m_accumulator -= m_delta_time;
        }

        // Rendering pipeline
        PrepareRender();
        // User Rendering
        Render();

        // GUI
        m_gui.Render();

        // Swap frame buffer
        m_window.SwapBuffers();

        // Update Frame Time
        //UpdateFrameTime();
    }

    return true;
}

bool Application::ShutDown()
{
    m_gui.Shutdown();
    m_window.Close();
    return true;
}

void Application::UpdateFrameTime()
{
    m_frame_timer += m_elapsed_time;
    m_frame_count++;
    if (m_frame_timer >= 1.0f)
    {
        m_last_fps = m_frame_count;
        m_frame_timer -= 1.0f;
        std::printf("INFO: Frame Time: %.4f FPS: %d\n", m_elapsed_time, m_frame_count);
        m_frame_count = 0;
    }
}

void Application::PrepareRender()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

// User Defined Functions
void Application::Create() {}
void Application::ProcessInput() {}
void Application::Simulate(f32 dt) {}
void Application::Render() {}