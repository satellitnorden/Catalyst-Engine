//Header file.
#include <Rendering/Engine/RenderPasses/BloomVerticalRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(BloomVerticalRenderPass);

/*
*	Default constructor.
*/
BloomVerticalRenderPass::BloomVerticalRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		BloomVerticalRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the bloom vertical render pass.
*/
void BloomVerticalRenderPass::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the main stage.
	SetMainStage(RenderPassMainStage::BloomVertical);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::BloomVertical);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::BloomVerticalFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::None);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetResolution());

	//Set the properties of the render pass.
	SetBlendEnabled(true);
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
		BloomVerticalRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Creates the render data table layout.
*/
void BloomVerticalRenderPass::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void BloomVerticalRenderPass::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->UpdateRenderDataTable(RenderDataTableUpdateInformation(0, RenderDataTableUpdateInformation::Type::RenderTarget, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneIntermediate)), _RenderDataTable);
}

/*
*	Renders the vertical bloom.
*/
void BloomVerticalRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Draw!
	commandBuffer->Draw(this, 4, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}