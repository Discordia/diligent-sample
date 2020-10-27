#pragma once

#define GLFW_EXPOSE_NATIVE_COCOA

#include <memory>
#include <string>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <core/RenderContext.h>

class GLFWWindow {
public:
    GLFWWindow(GLFWwindow *window, std::shared_ptr<RenderContext> renderContext);
    ~GLFWWindow();

    static std::unique_ptr<GLFWWindow> create(const std::string& title);

    bool running();
    void pollEvents();
    void swapBuffers();
    std::shared_ptr<RenderContext> getRenderContext();

private:
    GLFWwindow* window;
    std::shared_ptr<RenderContext> renderContext;
};