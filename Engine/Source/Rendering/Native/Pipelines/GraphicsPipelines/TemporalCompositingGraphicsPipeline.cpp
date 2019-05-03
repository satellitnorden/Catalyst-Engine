//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalCompositingGraphicsPipeline.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalCompositingGraphicsPipeline);

/*
*	Default constructor.
*/
TemporalCompositingGraphicsPipeline::TemporalCompositingGraphicsPipeline() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalCompositingGraphicsPipeline::Instance->InitializeInternal();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalCompositingGraphicsPipeline::Instance->RenderInternal();
	});
}

/*
*	Initializes the temporal compositing graphics pipeline.
*/
void TemporalCompositingGraphicsPipeline::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Set the main stage.
	SetMainStage(RenderPassStage::TemporalCompositing);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::TemporalCompositingFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution());

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
}

/*
*	Creates the render data table layout.
*/
void TemporalCompositingGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
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
void TemporalCompositingGraphicsPipeline::CreateRenderDataTables() NOEXCEPT
{
	_RenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);
	}
}

/*
*	Renders the temporal compositing.
*/
void TemporalCompositingGraphicsPipeline::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Retrieve the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Bind the correct accumulation buffer to the render data table.
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &currentRenderDataTable, _CurrentIndex == 0 ? RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2) : RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, currentRenderDataTable);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);

	//Update the current index.
	_CurrentIndex = _CurrentIndex == 0 ? 1 : 0;
}