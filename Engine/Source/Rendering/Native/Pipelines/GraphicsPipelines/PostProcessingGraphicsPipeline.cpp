//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/PostProcessingGraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Post processing fragment push constant data definition.
*/
class PostProcessingFragmentPushConstantData final
{

public:

	//The tint color.
	Vector3<float32> _TintColor;

	//Padding.
	Padding<4> _Padding;

	//The tint intensity.
	float32 _TintIntensity;

	//The chromatic aberration intensity.
	float32 _ChromaticAberrationIntensity;

	//The brightness.
	float32 _Brightness;

	//The contrast.
	float32 _Contrast;

	//The film grain intensity.
	float32 _FilmGrainIntensity;

	//The horizontal border.
	float32 _HorizontalBorder;

	//The saturation.
	float32 _Saturation;

};

/*
*	Initializes this graphics pipeline.
*/
void PostProcessingGraphicsPipeline::Initialize() NOEXCEPT
{
	//Reset this graphics pipeline.
	ResetGraphicsPipeline();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("PostProcessingFragmentShader")));

	//Add the output render targets.
	SetNumberOfOutputRenderTargets(1);
	AddOutputRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(PostProcessingFragmentPushConstantData));

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution(0));

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
}

/*
*	Executes this graphics pipeline.
*/
void PostProcessingGraphicsPipeline::Execute() NOEXCEPT
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
	PostProcessingFragmentPushConstantData data;

	data._TintColor = RenderingSystem::Instance->GetPostProcessingSystem()->GetTintColor();
	data._TintIntensity = RenderingSystem::Instance->GetPostProcessingSystem()->GetTintIntensity();
	data._ChromaticAberrationIntensity = RenderingSystem::Instance->GetPostProcessingSystem()->GetChromaticAberrationIntensity();
	data._Brightness = RenderingSystem::Instance->GetPostProcessingSystem()->GetBrightness();
	data._Contrast = RenderingSystem::Instance->GetPostProcessingSystem()->GetContrast();
	data._FilmGrainIntensity = RenderingSystem::Instance->GetPostProcessingSystem()->GetFilmGrainIntensity();
	data._HorizontalBorder = RenderingSystem::Instance->GetPostProcessingSystem()->GetHorizontalBorder();
	data._Saturation = RenderingSystem::Instance->GetPostProcessingSystem()->GetSaturation();

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(PostProcessingFragmentPushConstantData), &data);

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
void PostProcessingGraphicsPipeline::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);

	//Destroy the render data table layout.
	RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
}

/*
*	Creates the render data table layout.
*/
void PostProcessingGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
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
void PostProcessingGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_2), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}