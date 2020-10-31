#include <core/GLFWWindow.h>

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;

GLFWWindow::GLFWWindow(GLFWwindow *window)
    : window(window) {}

GLFWWindow::~GLFWWindow() {
    glfwTerminate();
}

unique_ptr<GLFWWindow> GLFWWindow::create(const string& title) {
    if (!glfwInit()) {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(640, 480, title.c_str(), nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return make_unique<GLFWWindow>(window);
}

bool GLFWWindow::running() {
    return !glfwWindowShouldClose(window);
}

void GLFWWindow::pollEvents() {
    glfwPollEvents();
}

void GLFWWindow::swapBuffers() {
    glfwSwapBuffers(window);
}

void* GLFWWindow::getWindowHandle() {
    return static_cast<void*>(glfwGetCocoaWindow(this->window));
}
