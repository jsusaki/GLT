#pragma once

#include "Window.h"

Window::Window() {}

void Window::Init(const std::string& title, s32 width, s32 height)
{
    m_title = title;
    m_width = width;
    m_height = height;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::printf("ERROR: Failed to initialize GLFW\n");
        glfwTerminate();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
    std::printf("INFO: GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (m_window == nullptr)
    {
        std::printf("ERROR: Failed to create GLFW window\n");
        glfwTerminate();
    }

    glfwSetWindowUserPointer(m_window, this);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetCursorPosCallback(m_window, cursor_position_callback);
    glfwSetScrollCallback(m_window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::printf("ERROR: GLAD Initialization Failed\n");
        glfwTerminate();
    }
}

void Window::SetInput(Input* input)
{
    m_input = input;
}

void Window::SetTitle(const std::string& title)
{
    glfwSetWindowTitle(m_window, m_title.c_str());
}

void Window::PollEvents() { glfwPollEvents(); }
bool Window::ShouldClose() { return glfwWindowShouldClose(m_window); }
void Window::SetShouldClose() { glfwSetWindowShouldClose(m_window, true); }
void Window::Close() { glfwDestroyWindow(m_window);  glfwTerminate(); }
void Window::SwapBuffers() { glfwSwapBuffers(m_window); }
void Window::Clear(const Color c)
{
    glm::vec4 v = to_float(c);
    glClearColor(v.r, v.g, v.b, v.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Window::GetWindow()
{
    return m_window;
}

Input* Window::GetInput()
{
    return m_input;
}
