//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/CompositingGraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void CompositingGraphicsPipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::CompositingFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));

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
*	Executes this graphics pipeline.
*/
void CompositingGraphicsPipeline::Execute() NOEXCEPT
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

/*
*	Creates the render data table layout.
*/
void CompositingGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 9> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void CompositingGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(3, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(4, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::AmbientOcclusion), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(5, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(6, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SpecularIrradiance), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(7, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DirectLighting), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(8, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R32G32B32A32_Float), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}