#include <core/GLFWWindow.h>
#include <core/Renderer.h>

int main() {
    auto window = GLFWWindow::create("Diligent Sample");
    auto renderContext = window->getRenderContext();
    auto renderer = Renderer::create(renderContext);

    while (window->running()) {
        renderer.render();
        window->swapBuffers();
        window->pollEvents();
    }
}

