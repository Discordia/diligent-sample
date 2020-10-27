#include <core/Renderer.h>

static const char* vertexShaderSource = R"(
struct PSInput
{
    float4 Pos   : SV_POSITION;
    float3 Color : COLOR;
};
void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn)
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);
    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue
    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)";

// Pixel shader simply outputs interpolated vertex color
static const char* pixelShaderSource = R"(
struct PSInput
{
    float4 Pos   : SV_POSITION;
    float3 Color : COLOR;
};
struct PSOutput
{
    float4 Color : SV_TARGET;
};
void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";

Renderer::Renderer(std::shared_ptr<RenderContext>  renderContext, RefCntAutoPtr<IPipelineState> pipelineState)
    : renderContext(std::move(renderContext)), pipelineState(std::move(pipelineState)) {}

Renderer Renderer::create(const std::shared_ptr<RenderContext>& renderContext) {
    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    RefCntAutoPtr<IPipelineState> pipelineState;

    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = renderContext->getSwapChain()->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = renderContext->getSwapChain()->GetDesc().DepthBufferFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.UseCombinedTextureSamplers = true;

    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle vertex shader";
        ShaderCI.Source = vertexShaderSource;
        renderContext->getRenderDevice()->CreateShader(ShaderCI, &pVS);
    }

    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle pixel shader";
        ShaderCI.Source = pixelShaderSource;
        renderContext->getRenderDevice()->CreateShader(ShaderCI, &pPS);
    }

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    renderContext->getRenderDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &pipelineState);
    return Renderer(renderContext, pipelineState);
}

void Renderer::render() {
    auto* pRTV = renderContext->getSwapChain()->GetCurrentBackBufferRTV();
    auto* pDSV = renderContext->getSwapChain()->GetDepthBufferDSV();
    renderContext->getDeviceContext()->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
    renderContext->getDeviceContext()->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    renderContext->getDeviceContext()->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0,RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    renderContext->getDeviceContext()->SetPipelineState(pipelineState);
    renderContext->getDeviceContext()->CommitShaderResources(nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;
    renderContext->getDeviceContext()->Draw(drawAttrs);
}
