//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/SkyDownsampleComputePipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Sky downsample push constant data definition.
*/
class SkyDownsamplePushConstantData final
{

public:

	//The input index.
	uint32 _InputIndex;

	//The output index.
	uint32 _OutputIndex;

};

/*
*	Initializes this compute pipeline.
*/
void SkyDownsampleComputePipeline::Initialize() NOEXCEPT
{
	//Set the shader.
	SetShader(Shader::SkyDownsampleCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Compute, 0, sizeof(SkyDownsamplePushConstantData));
}

/*
*	Executes this compute pipeline.
*/
void SkyDownsampleComputePipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer* const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Downsample all textures.
	for (uint32 i{ 0 }; i < CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES - 1; ++i)
	{
		//Push constants.
		SkyDownsamplePushConstantData data;

		data._InputIndex = i;
		data._OutputIndex = i + 1;

		command_buffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(SkyDownsamplePushConstantData), &data);

		//Dispatch!
		command_buffer->Dispatch(this, CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION >> (1 + i), CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION >> (1 + i), 6);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}