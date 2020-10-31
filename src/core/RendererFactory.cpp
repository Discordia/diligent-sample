#include <core/RendererFactory.h>

#include <vector>
#include <EngineFactoryOpenGL.h>
#include <Common/interface/RefCntAutoPtr.hpp>

using namespace Diligent;
using std::vector;

Renderer RendererFactory::create(void* windowHandle) {
    RefCntAutoPtr<IRenderDevice> device;
    RefCntAutoPtr<IDeviceContext> immediateContext;
    std::vector<RefCntAutoPtr<IDeviceContext>> deferredContexts;
    RefCntAutoPtr<ISwapChain> swapChain;

    SwapChainDesc swapChainDesc;
    swapChainDesc.BufferCount = 3;

    vector<IDeviceContext*> contexts;
    VERIFY_EXPR(pWindow != nullptr);

    auto* factoryOpenGL = GetEngineFactoryOpenGL();
    EngineGLCreateInfo createInfo;
    createInfo.Window = NativeWindow{windowHandle};

    if (createInfo.NumDeferredContexts != 0) {
        createInfo.NumDeferredContexts = 0;
    }

    contexts.resize(1 + createInfo.NumDeferredContexts);
    factoryOpenGL->CreateDeviceAndSwapChainGL(
            createInfo,
            &device,
            contexts.data(),
            swapChainDesc,
            &swapChain);

    immediateContext.Attach(contexts[0]);
    auto NumDeferredCtx = contexts.size() - 1;
    deferredContexts.resize(NumDeferredCtx);

    for (Uint32 ctx = 0; ctx < NumDeferredCtx; ++ctx) {
        deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }

    return Renderer(device, immediateContext, swapChain);
}

