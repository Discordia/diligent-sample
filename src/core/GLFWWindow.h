#pragma once

#define GLFW_EXPOSE_NATIVE_COCOA

#include <memory>
#include <string>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <core/RendererFactory.h>

class GLFWWindow {
public:
    explicit GLFWWindow(GLFWwindow *window);
    ~GLFWWindow();

    static std::unique_ptr<GLFWWindow> create(const std::string& title);

    bool  running();
    void  pollEvents();
    void  swapBuffers();
    void* getWindowHandle();

private:
    GLFWwindow* window;
};