//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingTemporalDenoisingGraphicsPipeline.h>

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

	//The source render target index.
	uint32 _SourceRenderTargetIndex;

	//The scene features 4 render target index.
	uint32 _SceneFeatures4RenderTargetIndex;

};

/*
*	Initializes this graphics pipeline.
*/
void IndirectLightingTemporalDenoisingGraphicsPipeline::Initialize(	const uint32 source_render_target_index,
																	const uint32 scene_features_4_render_target_index,
																	const RenderTargetHandle previous_temporal_buffer,
																	const RenderTargetHandle target_1,
																	const RenderTargetHandle target_2,
																	const Resolution render_resolution) NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable(previous_temporal_buffer);

	//Set the source render target index.
	_SourceRenderTargetIndex = source_render_target_index;

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
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(IndirectLightingTemporalDenoisingPushConstantData));

	//Set the render resolution.
	SetRenderResolution(render_resolution);

	//Set the properties of the render pass.
	SetDepthStencilAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetDepthStencilAttachmentStoreOperator(AttachmentStoreOperator::DONT_CARE);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::DONT_CARE);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::STORE);
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Indirect Lighting Temporal Denoising");
#endif
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
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	IndirectLightingTemporalDenoisingPushConstantData data;

	data._Delta = Vector2<float32>(1.0f / static_cast<float32>(GetRenderResolution()._Width), 1.0f / static_cast<float32>(GetRenderResolution()._Height));
	data._SourceRenderTargetIndex = _SourceRenderTargetIndex;
	data._SceneFeatures4RenderTargetIndex = _SceneFeatures4RenderTargetIndex;

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(IndirectLightingTemporalDenoisingPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Terminates this graphics pipeline.
*/
void IndirectLightingTemporalDenoisingGraphicsPipeline::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	if (_RenderDataTable)
	{
		RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);
		_RenderDataTable = EMPTY_HANDLE;
	}

	//Destroy the render data table layout.
	if (_RenderDataTableLayout)
	{
		RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
		_RenderDataTableLayout = EMPTY_HANDLE;
	}
}

/*
*	Creates the render data table layout.
*/
void IndirectLightingTemporalDenoisingGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void IndirectLightingTemporalDenoisingGraphicsPipeline::CreateRenderDataTable(const RenderTargetHandle previous_temporal_buffer) NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, previous_temporal_buffer, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}