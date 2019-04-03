//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AntiAliasingPipeline.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AntiAliasingPipeline);

/*
*	Default constructor.
*/
AntiAliasingPipeline::AntiAliasingPipeline() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		AntiAliasingPipeline::Instance->InitializeInternal();
	});
}

/*
*	Initializes the anti-aliasing pipeline.
*/
void AntiAliasingPipeline::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the main stage.
	SetMainStage(PipelineMainStage::AntiAliasing);

	//Set the sub stage.
	SetSubStage(PipelineSubStage::AntiAliasing);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::AntiAliasingFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Screen);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::Back);
	SetDepthCompareOperator(CompareOperator::Always);
	SetDepthTestEnabled(false);
	SetDepthWriteEnabled(false);
	SetStencilTestEnabled(false);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Always);
	SetStencilCompareMask(0);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(0);
	SetTopology(Topology::TriangleFan);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		AntiAliasingPipeline::Instance->RenderInternal();
	});

	//Initialize the pipeline.
	RenderingSystem::Instance->InitializePipeline(this);
}


/*
*	Creates the render data table layout.
*/
void AntiAliasingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void AntiAliasingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}

/*
*	Renders the anti-aliasing.
*/
void AntiAliasingPipeline::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}