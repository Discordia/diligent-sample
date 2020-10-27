#pragma once

#include <core/RenderContext.h>

class Renderer {
public:
    Renderer(std::shared_ptr<RenderContext>  renderContext, RefCntAutoPtr<IPipelineState> pipelineState);

    ~Renderer() = default;

    static Renderer create(const std::shared_ptr<RenderContext>& renderContext);

    void render();

private:
    std::shared_ptr<RenderContext> renderContext;
    RefCntAutoPtr<IPipelineState> pipelineState;
};