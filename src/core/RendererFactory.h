#pragma once

#include <core/Renderer.h>

class RendererFactory {
public:
    static Renderer create(void* windowHandle);
};