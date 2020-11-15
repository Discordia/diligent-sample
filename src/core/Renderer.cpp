#include <core/Renderer.h>

Renderer::Renderer(const RefCntAutoPtr<IRenderDevice>& renderDevice,
                   const RefCntAutoPtr<IDeviceContext>& deviceContext,
                   const RefCntAutoPtr<ISwapChain>& swapChain)
    : renderDevice(renderDevice),
    deviceContext(deviceContext),
    swapChain(swapChain) {}

void Renderer::init() {
    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = swapChain->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = swapChain->GetDesc().DepthBufferFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;

    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.UseCombinedTextureSamplers = true;

    auto* factoryOpenGL = GetEngineFactoryOpenGL();
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    factoryOpenGL->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle vertex shader";
        ShaderCI.FilePath = "assets/triangle.vsh";
        renderDevice->CreateShader(ShaderCI, &pVS);
    }

    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle pixel shader";
        ShaderCI.FilePath = "assets/triangle.psh";
        renderDevice->CreateShader(ShaderCI, &pPS);
    }

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    renderDevice->CreateGraphicsPipelineState(PSOCreateInfo, &pipelineState);
}

void Renderer::render() {
    auto* pRTV = swapChain->GetCurrentBackBufferRTV();
    auto* pDSV = swapChain->GetDepthBufferDSV();
    deviceContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
    deviceContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0,RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    deviceContext->SetPipelineState(pipelineState);
    deviceContext->CommitShaderResources(nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;
    deviceContext->Draw(drawAttrs);
}