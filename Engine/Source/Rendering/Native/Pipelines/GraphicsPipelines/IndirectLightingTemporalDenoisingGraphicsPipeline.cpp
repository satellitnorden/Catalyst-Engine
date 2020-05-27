//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingTemporalDenoisingGraphicsPipeline.h>

//Core.
#include <Core/General/Perceiver.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Indirect lighting temporal denoising push constant data definition.
*/
class IndirectLightingTemporalDenoisingPushConstantData final
{

public:

	//The delta.
	Vector2<float32> _Delta;

	//The first source render target index.
	uint32 _SourceRenderTargetIndex1;

	//The second source render target index.
	uint32 _SourceRenderTargetIndex2;

	//The scene features 4 render target index.
	uint32 _SceneFeatures4RenderTargetIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void IndirectLightingTemporalDenoisingGraphicsPipeline::Initialize(	const uint32 source_render_target_index_1,
																	const uint32 source_render_target_index_2,
																	const uint32 scene_features_4_render_target_index,
																	const RenderTargetHandle target_1,
																	const RenderTargetHandle target_2,
																	const Resolution render_resolution) NOEXCEPT
{
	//Set the first source render target index.
	_SourceRenderTargetIndex1 = source_render_target_index_1;

	//Set the second source render target index.
	_SourceRenderTargetIndex2 = source_render_target_index_2;

	//Set the scene features 4 render target index.
	_SceneFeatures4RenderTargetIndex = scene_features_4_render_target_index;

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("IndirectLightingTemporalDenoisingFragmentShader")));

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(2);
	AddOutputRenderTarget(target_1);
	AddOutputRenderTarget(target_2);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(IndirectLightingTemporalDenoisingPushConstantData));

	//Set the render resolution.
	SetRenderResolution(render_resolution);

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
void IndirectLightingTemporalDenoisingGraphicsPipeline::Execute() NOEXCEPT
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
	IndirectLightingTemporalDenoisingPushConstantData data;

	data._Delta = Vector2<float32>(1.0f / static_cast<float32>(GetRenderResolution()._Width), 1.0f / static_cast<float32>(GetRenderResolution()._Height));
	data._SourceRenderTargetIndex1 = _SourceRenderTargetIndex1;
	data._SourceRenderTargetIndex2 = _SourceRenderTargetIndex2;
	data._SceneFeatures4RenderTargetIndex = _SceneFeatures4RenderTargetIndex;

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(IndirectLightingTemporalDenoisingPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}