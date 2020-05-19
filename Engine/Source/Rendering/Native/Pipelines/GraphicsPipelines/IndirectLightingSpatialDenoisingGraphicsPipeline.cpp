//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingSpatialDenoisingGraphicsPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Indirect lighting spatial denoising push constant data definition.
*/
class IndirectLightingSpatialDenoisingPushConstantData final
{

public:

	//The inverse resolution.
	Vector2<float32> _InverseResolution;

	//The source render target index.
	uint32 _SourceRenderTargetIndex;

	//The stride.
	int32 _Stride;

};

/*
*	Initializes this graphics pipeline.
*/
void IndirectLightingSpatialDenoisingGraphicsPipeline::Initialize(const uint32 source_render_target_index, const int32 stride, const RenderTargetHandle target) NOEXCEPT
{
	//Set the source render target index.
	_SourceRenderTargetIndex = source_render_target_index;

	//Set the stride.
	_Stride = stride;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("IndirectLightingSpatialDenoisingFragmentShader")));

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(target);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(IndirectLightingSpatialDenoisingPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(1));

	//Set the properties of the render pass.
	SetShouldClear(false);
	SetBlendEnabled(false);
	SetBlendFactorSourceColor(BlendFactor::SourceAlpha);
	SetBlendFactorDestinationColor(BlendFactor::OneMinusSourceAlpha);
	SetBlendFactorSourceAlpha(BlendFactor::One);
	SetBlendFactorDestinationAlpha(BlendFactor::Zero);
	SetCullMode(CullMode::None);
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
void IndirectLightingSpatialDenoisingGraphicsPipeline::Execute() NOEXCEPT
{
	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Push constants.
	IndirectLightingSpatialDenoisingPushConstantData data;

	data._InverseResolution = Vector2<float32>(1.0f / static_cast<float>(RenderingSystem::Instance->GetScaledResolution(1)._Width), 1.0f / static_cast<float32>(RenderingSystem::Instance->GetScaledResolution(1)._Height));
	data._SourceRenderTargetIndex = _SourceRenderTargetIndex;
	data._Stride = _Stride;

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(IndirectLightingSpatialDenoisingPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}