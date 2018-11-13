//Header file.
#include <Rendering/Engine/RenderPasses/LightingRenderPass.h>

//Rendering.
#include <Rendering/Engine/CommandBuffer.h>

//Managers.
#include <Managers/EnvironmentManager.h>
#include <Managers/RenderingConfigurationManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LightingRenderPass);

/*
*	Default constructor.
*/
LightingRenderPass::LightingRenderPass() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		LightingRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the lighting render pass.
*/
void LightingRenderPass::InitializeInternal() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the main stage.
	SetMainStage(RenderPassMainStage::Scene);

	//Set the sub stage.
	SetSubStage(RenderPassSubStage::Lighting);

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::LightingFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::Scene);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::DynamicUniformData));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::EnvironmentMaterial));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::EnvironmentMaterial));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(int32));

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
	SetStencilCompareMask(BIT(0));
	SetStencilWriteMask(0);
	SetStencilReferenceMask(BIT(0));
	SetTopology(Topology::TriangleFan);

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		LightingRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Creates the render data table layout.
*/
void LightingRenderPass::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 4> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment), 
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void LightingRenderPass::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, _RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferAlbedo), RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge)));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, _RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferNormalDepth), RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge)));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(2, _RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneBufferMaterialProperties), RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToEdge)));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(3, _RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectionalShadow), RenderingSystem::Instance->GetSampler(SamplerProperties(TextureFilter::Nearest, MipmapMode::Nearest, AddressMode::ClampToBorder)));
}

/*
*	Renders the lighting.
*/
void LightingRenderPass::RenderInternal() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetCurrentDynamicUniformDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, EnvironmentManager::Instance->GetNightEnvironmentMaterial()._RenderDataTable);
	commandBuffer->BindRenderDataTable(this, 2, EnvironmentManager::Instance->GetDayEnvironmentMaterial()._RenderDataTable);
	commandBuffer->BindRenderDataTable(this, 3, _RenderDataTable);

	//Pust constants.
	constexpr int32 screenSpaceAmbientOcclusionEnabled{ false };
	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(int32), &screenSpaceAmbientOcclusionEnabled);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}