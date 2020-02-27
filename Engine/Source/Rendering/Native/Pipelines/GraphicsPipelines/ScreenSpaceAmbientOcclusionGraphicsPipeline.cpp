//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceAmbientOcclusionGraphicsPipeline.h>

//Math.
#include <Math/Noise/HammersleySequence.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes this graphics pipeline.
*/
void ScreenSpaceAmbientOcclusionGraphicsPipeline::Initialize() NOEXCEPT
{
	//Create the samples uniform buffer.
	CreateSamplesUniformBuffer();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shaders.
	SetVertexShader(Shader::ViewportVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ScreenSpaceAmbientOcclusionFragment);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderingSystem::Instance->GetRenderTarget(RenderTarget::AmbientOcclusion));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the render resolution.
	SetRenderResolution(RenderingSystem::Instance->GetScaledResolution() / 2);

	//Set the properties of the render pass.
	SetShouldClear(false);
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
void ScreenSpaceAmbientOcclusionGraphicsPipeline::Execute() NOEXCEPT
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
*	Creates the samples uniform buffer.
*/
void ScreenSpaceAmbientOcclusionGraphicsPipeline::CreateSamplesUniformBuffer() NOEXCEPT
{
	//Calculate the hemisphere samples.
	StaticArray<Vector4<float32>, 64> hemisphere_samples;

	uint8 counter{ 0 };

	for (uint8 i{ 0 }; i < 8; ++i)
	{
		for (uint8 j{ 0 }; j < 8; ++j)
		{
			hemisphere_samples[counter] = Vector4<float32>(HammersleySequence::CalculateCoordinateHemisphere(i + j * 8 + 1, 65), HammersleySequence::RadicalInverse(counter + 1));

			++counter;
		}
	}

	//Create the buffer and upload the data.
	const void* const RESTRICT data_chunks[]{ hemisphere_samples.Data() };
	const uint64 data_sizes[]{ hemisphere_samples.Size() * sizeof(Vector4<float32>) };
	RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &_SamplesUniformBuffer);
	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_SamplesUniformBuffer);
}

/*
*	Creates the render data table layout.
*/
void ScreenSpaceAmbientOcclusionGraphicsPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void ScreenSpaceAmbientOcclusionGraphicsPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);
	
	RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &_RenderDataTable, _SamplesUniformBuffer);
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2_Half), RenderingSystem::Instance->GetSampler(Sampler::FilterNearest_MipmapModeNearest_AddressModeClampToEdge));
}