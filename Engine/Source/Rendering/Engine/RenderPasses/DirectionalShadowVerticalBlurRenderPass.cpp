//Header file.
#include <Rendering/Engine/RenderPasses/DirectionalShadowVerticalBlurRenderPass.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalShadowVerticalBlurRenderPass);

/*
*	Default constructor.
*/
DirectionalShadowVerticalBlurRenderPass::DirectionalShadowVerticalBlurRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		DirectionalShadowVerticalBlurRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the directional shadow vertical blur render pass.
*/
void DirectionalShadowVerticalBlurRenderPass::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();
	
	//Initialize the gaussian blur data.
	InitializeGaussianBlurData();

	//Set the main stage.
	SetMainStage(RenderPassMainStage::DirectionalShadowVerticalBlur);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::DirectionalShadowVerticalBlur);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::GaussianBlurFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::DirectionalShadow);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(GaussianBlurData));

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
	SetStencilTestEnabled(true);
	SetStencilFailOperator(StencilOperator::Keep);
	SetStencilPassOperator(StencilOperator::Keep);
	SetStencilDepthFailOperator(StencilOperator::Keep);
	SetStencilCompareOperator(CompareOperator::Equal);
	SetStencilCompareMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetStencilWriteMask(0);
	SetStencilReferenceMask(RenderingConstants::SCENE_BUFFER_STENCIL_BIT);
	SetTopology(Topology::TriangleFan);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		DirectionalShadowVerticalBlurRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Creates the render data table layout.
*/
void DirectionalShadowVerticalBlurRenderPass::CreateRenderDataTableLayout() NOEXCEPT
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
void DirectionalShadowVerticalBlurRenderPass::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, _RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}

/*
*	Initializes the gaussian blur data.
*/
void DirectionalShadowVerticalBlurRenderPass::InitializeGaussianBlurData() NOEXCEPT
{
	_GaussianBlurData._Direction = Vector2<float>(0.0f, 1.0f);
	_GaussianBlurData._InverseResolution = Vector2<float>(1.0f / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width), 1.0f / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height));
}

/*
*	Renders the directional shadow vertical blur.
*/
void DirectionalShadowVerticalBlurRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(GaussianBlurData), &_GaussianBlurData);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}