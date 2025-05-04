#pragma once

#include <print>
#include <glad/glad.h>
#include <glfw3.h>
#include <stb_image.h>

#include "Common.h"
#include "Input.h"
#include "Graphics/Color.h"

class Window
{
public:
    Window();

public:
    void Init(const std::string& title = "GL Template", s32 width = 800, s32 height = 600);
    void SetInput(Input* input);

    void SetTitle(const std::string& title);

    void PollEvents();
    bool ShouldClose();
    void SetShouldClose();
    void Close();
    void SwapBuffers();
    void Clear(const Color c);

    GLFWwindow* GetWindow();
    Input* GetInput();

private:
    GLFWwindow* m_window;
    Input* m_input;

    std::string m_title;
    s32 m_width;
    s32 m_height;
};

// GLFW callback definitions
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void glfw_error_callback(int error, const char* description)
{
    std::fprintf(stderr, "ERROR: GLFW %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self->GetInput()) return;

    if (action == GLFW_PRESS)        self->GetInput()->SetKey(key, true);
    else if (action == GLFW_RELEASE) self->GetInput()->SetKey(key, false);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self->GetInput()) return;

    if (action == GLFW_PRESS)        self->GetInput()->SetButton(button, true);
    else if (action == GLFW_RELEASE) self->GetInput()->SetButton(button, false);
}

static void cursor_position_callback(GLFWwindow* window, f64 x, f64 y)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self->GetInput()) return;

    self->GetInput()->SetMousePos(x, y);
}

static void scroll_callback(GLFWwindow* window, f64 dx, f64 dy)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self->GetInput()) return;

    for (u32 i = 0; i < std::abs(dy); i++)
    {
        if (dy > 0)      self->GetInput()->SetMouseWheelDelta(static_cast<f32>(1.0f));
        else if (dy < 0) self->GetInput()->SetMouseWheelDelta(static_cast<f32>(-1.0f));
    }
}