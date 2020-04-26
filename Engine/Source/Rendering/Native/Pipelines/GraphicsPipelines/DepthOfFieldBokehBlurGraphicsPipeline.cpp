//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldBokehBlurGraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Depth of field bokeh blur fragment push constant data definition.
*/
class DepthOfFieldBokehBlurFragmentPushConstantData final
{

public:

	//The depth of field focus distance.
	float32 _DepthOfFieldFocusDistance;

	//The depth of field size.
	float32 _DepthOfFieldSize;

};

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Prints the sample points.
*/
void PrintSamplePoints() NOEXCEPT
{
	//Define constants.
	constexpr uint8 RESOLUTION{ 3 };

	//Calculate all sample points.
	DynamicArray<Vector2<float32>> sample_points;

	for (int8 X{ -RESOLUTION }; X <= RESOLUTION; ++X)
	{
		for (int8 Y{ -RESOLUTION }; Y <= RESOLUTION; ++Y)
		{
			//Calculate the normalized coordinate.
			const Vector2<float32> normalized_coordinate{ static_cast<float32>(X) / static_cast<float32>(RESOLUTION), static_cast<float32>(Y) / static_cast<float32>(RESOLUTION) };

			//Skip this coordinate if it is not within the unit disk.
			if (normalized_coordinate.LengthSquared() <= 1.0f)
			{
				sample_points.Emplace(normalized_coordinate);
			}
		}
	}

	//Print the sample points.
	PRINT_TO_OUTPUT("#define DEPTH_OF_FIELD_BOKEH_BLUR_SAMPLES (" << sample_points.Size() << ")");
	PRINT_TO_OUTPUT("");
	PRINT_TO_OUTPUT("vec2 SAMPLE_POINTS[" << sample_points.Size() << "] = vec2[]");
	PRINT_TO_OUTPUT("(");

	for (const Vector2<float32> &sample_point : sample_points)
	{
		PRINT_TO_OUTPUT("\tvec2(" << sample_point._X << "f, " << sample_point._Y << "f),");
	}

	PRINT_TO_OUTPUT(");");
}
#endif

/*
*	Initializes this graphics pipeline.
*/
void DepthOfFieldBokehBlurGraphicsPipeline::Initialize() NOEXCEPT
{
#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Print the sample points.
	//PrintSamplePoints();
#endif

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(ResourceSystem::Instance->GetShaderResource(HashString("ViewportVertexShader")));
	SetTessellationControlShader(ResourcePointer<ShaderResource>());
	SetTessellationEvaluationShader(ResourcePointer<ShaderResource>());
	SetGeometryShader(ResourcePointer<ShaderResource>());
	SetFragmentShader(ResourceSystem::Instance->GetShaderResource(HashString("DepthOfFieldBokehBlurFragmentShader")));

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::FRAGMENT, 0, sizeof(DepthOfFieldBokehBlurFragmentPushConstantData));

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
void DepthOfFieldBokehBlurGraphicsPipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	DepthOfFieldBokehBlurFragmentPushConstantData data;

	data._DepthOfFieldFocusDistance = RenderingSystem::Instance->GetPostProcessingSystem()->GetDepthOfFieldFocusDistance();
	data._DepthOfFieldSize = RenderingSystem::Instance->GetPostProcessingSystem()->GetDepthOfFieldSize();

	command_buffer->PushConstants(this, ShaderStage::FRAGMENT, 0, sizeof(DepthOfFieldBokehBlurFragmentPushConstantData), &data);

	//Draw!
	command_buffer->Draw(this, 3, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void DepthOfFieldBokehBlurGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::FRAGMENT)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void DepthOfFieldBokehBlurGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
}