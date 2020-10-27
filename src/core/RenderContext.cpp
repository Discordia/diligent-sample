#include <core/RenderContext.h>

using std::vector;

RenderContext::RenderContext(
        RefCntAutoPtr<IRenderDevice> renderDevice,
        RefCntAutoPtr<IDeviceContext> deviceContext,
        RefCntAutoPtr<ISwapChain> swapChain)
            : renderDevice(std::move(renderDevice)),
            deviceContext(std::move(deviceContext)),
            swapChain(std::move(swapChain)) {}

RenderContext::~RenderContext() = default;

RenderContext RenderContext::create(const NativeWindow *nativeWindow) {
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
    createInfo.Window = *nativeWindow;

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

    return RenderContext(device, immediateContext, swapChain);
}

RefCntAutoPtr<ISwapChain> RenderContext::getSwapChain() {
    return swapChain;
}

RefCntAutoPtr<IRenderDevice> RenderContext::getRenderDevice() {
    return renderDevice;
}

RefCntAutoPtr<IDeviceContext> RenderContext::getDeviceContext() {
    return deviceContext;
}
