#pragma once

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 1
#endif

#ifndef PLATFORM_MACOS
#    define PLATFORM_MACOS 1
#endif

#include <memory>
#include <EngineFactoryOpenGL.h>
#include <Common/interface/RefCntAutoPtr.hpp>

using namespace Diligent;

class Renderer {
public:
    Renderer(const RefCntAutoPtr<IRenderDevice>& renderDevice,
             const RefCntAutoPtr<IDeviceContext>& deviceContext,
             const RefCntAutoPtr<ISwapChain>& swapChain);
    ~Renderer() = default;

    void init();
    void render();

private:
    RefCntAutoPtr<ISwapChain> swapChain;
    RefCntAutoPtr<IDeviceContext> deviceContext;
    RefCntAutoPtr<IRenderDevice> renderDevice;
    RefCntAutoPtr<IPipelineState> pipelineState;
};