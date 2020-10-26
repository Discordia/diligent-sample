#include <memory>
#include <vector>

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 1
#endif

#ifndef PLATFORM_MACOS
#    define PLATFORM_MACOS 1
#endif

#include <EngineFactoryOpenGL.h>
#include <Common/interface/RefCntAutoPtr.hpp>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace Diligent;

void InitializeEngine(const NativeWindow* pWindow);
void InitializePipeline();
void Render();

RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_GL;
RefCntAutoPtr<IPipelineState> m_pPSO;
RefCntAutoPtr<IEngineFactory> m_pEngineFactory;
RefCntAutoPtr<IRenderDevice> m_pDevice;
RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
std::vector<RefCntAutoPtr<IDeviceContext>> m_pDeferredContexts;
RefCntAutoPtr<ISwapChain> m_pSwapChain;
std::vector<DisplayModeAttribs> m_DisplayModes;

int m_InitialWindowWidth = 0;
int m_InitialWindowHeight = 0;
int m_ValidationLevel = -1;
std::string m_AppTitle;
Uint32 m_AdapterId = 0;
ADAPTER_TYPE m_AdapterType = ADAPTER_TYPE_UNKNOWN;
int m_SelectedDisplayMode = 0;
bool m_bVSync = false;
bool m_bFullScreenMode = false;
bool m_bShowAdaptersDialog = true;
double m_CurrentTime = 0;

static const char* VSSource = R"(
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
static const char* PSSource = R"(
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

void InitializeEngine(const NativeWindow* pWindow) {
    SwapChainDesc SCDesc;
    SCDesc.BufferCount = 3;

    std::vector<IDeviceContext*> ppContexts;
    VERIFY_EXPR(pWindow != nullptr);
    auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
    m_pEngineFactory = pFactoryOpenGL;
    EngineGLCreateInfo CreationAttribs;
    CreationAttribs.Window = *pWindow;

    if (CreationAttribs.NumDeferredContexts != 0) {
        CreationAttribs.NumDeferredContexts = 0;
    }

    ppContexts.resize(1 + CreationAttribs.NumDeferredContexts);
    pFactoryOpenGL->CreateDeviceAndSwapChainGL(CreationAttribs, &m_pDevice,
                                             ppContexts.data(), SCDesc,
                                             &m_pSwapChain);

    m_pImmediateContext.Attach(ppContexts[0]);
    auto NumDeferredCtx = ppContexts.size() - 1;
    m_pDeferredContexts.resize(NumDeferredCtx);

    for (Uint32 ctx = 0; ctx < NumDeferredCtx; ++ctx)
        m_pDeferredContexts[ctx].Attach(ppContexts[1 + ctx]);
}

void InitializePipeline() {

    // Pipeline state object encompasses configuration of all GPU stages

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
    // Use the depth buffer format from the swap chain
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // No back face culling for this tutorial
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    // Disable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture
    // + g_Texture_sampler combination)
    ShaderCI.UseCombinedTextureSamplers = true;
    // Create a vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle vertex shader";
        ShaderCI.Source = VSSource;
        m_pDevice->CreateShader(ShaderCI, &pVS);
    }

    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle pixel shader";
        ShaderCI.Source = PSSource;
        m_pDevice->CreateShader(ShaderCI, &pPS);
    }

    // Finally, create the pipeline state
    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
}

void Render() {
    auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
    m_pImmediateContext->SetRenderTargets(
            1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Clear the back buffer
    const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
    // Let the engine perform required state transitions
    m_pImmediateContext->ClearRenderTarget(
            pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(
            pDSV, CLEAR_DEPTH_FLAG, 1.f, 0,
            RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Set pipeline state in the immediate context
    m_pImmediateContext->SetPipelineState(m_pPSO);
    // We need to commit shader resource. Even though in this example
    // we don't really have any resources, this call also sets the shaders
    m_pImmediateContext->CommitShaderResources(
            nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;  // We will render 3 vertices
    m_pImmediateContext->Draw(drawAttrs);
}

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "GLFW CMake starter", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    void* window_handle;
    glfwMakeContextCurrent(window);
    window_handle = static_cast<void*>(glfwGetCocoaWindow(window));

    NativeWindow nw{window_handle};
    InitializeEngine(&nw);
    InitializePipeline();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        if (m_pImmediateContext) {
            Render();

            glfwSwapBuffers(window);
        }

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
