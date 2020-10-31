#include <core/GLFWWindow.h>
#include <core/RendererFactory.h>

int main() {
    auto window = GLFWWindow::create("Diligent Sample");
    auto renderer = RendererFactory::create(window->getWindowHandle());
    renderer.init();

    while (window->running()) {
        renderer.render();
        window->swapBuffers();
        window->pollEvents();
    }
}

