#pragma once

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 1
#endif

#ifndef PLATFORM_MACOS
#    define PLATFORM_MACOS 1
#endif

#include <vector>
#include <EngineFactoryOpenGL.h>
#include <Common/interface/RefCntAutoPtr.hpp>

using namespace Diligent;

class RenderContext {
public:
    RenderContext(
            RefCntAutoPtr<IRenderDevice> renderDevice,
            RefCntAutoPtr<IDeviceContext> deviceContext,
            RefCntAutoPtr<ISwapChain> swapChain);
    ~RenderContext();

    static RenderContext create(const NativeWindow* pWindow);

    RefCntAutoPtr<ISwapChain> getSwapChain();
    RefCntAutoPtr<IRenderDevice> getRenderDevice();
    RefCntAutoPtr<IDeviceContext> getDeviceContext();

private:
    RefCntAutoPtr<ISwapChain> swapChain;
    RefCntAutoPtr<IDeviceContext> deviceContext;
    RefCntAutoPtr<IRenderDevice> renderDevice;
};