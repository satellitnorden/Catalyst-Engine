//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AmbientOcclusionTemporalDenoisingGraphicsPipeline.h>

//Core.
#include <Core/General/Perceiver.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Ambient occlsion temporal denoising push constant data definition.
*/
class  AmbientOcclusionTemporalDenoisingPushConstantData final
{

public:

	//The previous render target index.
	uint32 _PreviousRenderTargetIndex;

	//The current render target index.
	uint32 _CurrentRenderTargetIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void AmbientOcclusionTemporalDenoisingGraphicsPipeline::Initialize(	const uint32 source_render_target_index,
																	const RenderTargetHandle target) NOEXCEPT
{
	//Set the source render target index.
	_SourceRenderTargetIndex = source_render_target_index;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("AmbientOcclusionTemporalDenoisingFragmentShader")));

	//Add the render targets.
	SetNumberOfRenderTargets(2);
	AddRenderTarget(target);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::AMBIENT_OCCLUSION));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(AmbientOcclusionTemporalDenoisingPushConstantData));

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
void AmbientOcclusionTemporalDenoisingGraphicsPipeline::Execute() NOEXCEPT
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
	AmbientOcclusionTemporalDenoisingPushConstantData data;

	data._PreviousRenderTargetIndex = _SourceRenderTargetIndex;
	data._CurrentRenderTargetIndex = CatalystShaderConstants::AMBIENT_OCCLUSION_RENDER_TARGET_INDEX;

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(AmbientOcclusionTemporalDenoisingPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}