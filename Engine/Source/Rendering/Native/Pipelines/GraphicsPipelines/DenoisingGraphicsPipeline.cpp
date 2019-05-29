//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DenoisingGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The direction.
	Vector2<float> _Direction;

	//The stride.
	float _Stride;

};

/*
*	Initializes this graphics pipeline.
*/
void DenoisingGraphicsPipeline::Initialize(const Direction direction, const float stride, const RenderTargetHandle source, const RenderTargetHandle target) NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTables(source);

	//Set the direction.
	_Direction = direction;

	//Set the stride.
	_Stride = stride;

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::DenoisingFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(target);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Fragment, 0, sizeof(PushConstantData));

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
void DenoisingGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 4> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void DenoisingGraphicsPipeline::CreateRenderDataTables(const RenderTargetHandle source) NOEXCEPT
{
	_RenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);

		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &renderDataTable, source, RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &renderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(2, 0, &renderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	}
}

/*
*	Executes this graphics pipeline.
*/
void DenoisingGraphicsPipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Update the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(3, 0, &currentRenderDataTable, _CurrentBufferIndex == 0 ? RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer1) : RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationDescriptionBuffer2), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, currentRenderDataTable);

	//Push constants.
	PushConstantData data;

	if (_Direction == Direction::Horizontal)
	{
		data._Direction = Vector2<float>(1.0f / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width), 0.0f);
	}

	else
	{
		data._Direction = Vector2<float>(0.0f, 1.0f / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height));
	}

	data._Stride = _Stride;

	commandBuffer->PushConstants(this, ShaderStage::Fragment, 0, sizeof(PushConstantData), &data);

	//Draw!
	commandBuffer->Draw(this, 3, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == 0 ? 1 : 0;
}