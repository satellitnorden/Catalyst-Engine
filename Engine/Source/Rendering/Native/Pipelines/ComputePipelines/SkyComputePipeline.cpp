//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/SkyComputePipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Sky push constant data definition.
*/
class SkyPushConstantData final
{

public:

	//The upper sky color.
	Vector3<float> _UpperSkyColor;

	//Padding.
	Padding<4> _Padding1;

	//The lower sky color.
	Vector3<float> _LowerSkyColor;

	//Padding.
	Padding<4> _Padding2;

	//The sky light direction.
	Vector3<float> _SkyLightDirection;

	//Padding.
	Padding<4> _Padding3;

	//The sky light luminance.
	Vector3<float> _SkyLightLuminance;

	//Padding.
	Padding<4> _Padding4;

	//The sky light intensity.
	float32 _SkyLightIntensity;

	//The current iteration.
	uint32 _CurrentIteration;

	//The parameters difference.
	float _ParametersDifference;

	//The star intensity.
	float _StarIntensity;

};

/*
*	Initializes this compute pipeline.
*/
void SkyComputePipeline::Initialize() NOEXCEPT
{
	//Set the shader.
	SetShader(Shader::SkyCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(1);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Compute, 0, sizeof(SkyPushConstantData));
}

/*
*	Executes this compute pipeline.
*/
void SkyComputePipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer* const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	//Push constants.
	SkyPushConstantData data;

	const SkyGradient current_sky_gradient{ WorldSystem::Instance->GetSkySystem()->GetCurrentSkyGradient() };
	data._UpperSkyColor = current_sky_gradient._UpperSkyColor;
	data._LowerSkyColor = current_sky_gradient._LowerSkyColor;

	//Pick the first directional light.
	data._SkyLightDirection = VectorConstants::UP;
	data._SkyLightLuminance = VectorConstants::ZERO;
	data._SkyLightIntensity = 0.0f;

	for (uint64 i{ 0 }, size{ ComponentManager::GetNumberOfLightComponents() }; i < size; ++i)
	{
		const LightComponent& component{ ComponentManager::GetLightLightComponents()[i] };

		if (static_cast<LightType>(component._LightType) == LightType::DIRECTIONAL)
		{
			data._SkyLightDirection = component._Direction;
			data._SkyLightLuminance = component._Color;
			data._SkyLightIntensity = component._Intensity;

			break;
		}
	}

	data._CurrentIteration = _CurrentIteration;
	data._ParametersDifference = CalculateParametersDifference();
	data._StarIntensity = WorldSystem::Instance->GetSkySystem()->GetCurrentStarIntensity();

	command_buffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(SkyPushConstantData), &data);

	//Dispatch!
	command_buffer->Dispatch(this, CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION >> 1, CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION >> 1, 6);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);

	//Update the current iteration.
	_CurrentIteration = (_CurrentIteration + 1) & 3;
}

/*
*	Calculates the parameters difference.
*/
NO_DISCARD float SkyComputePipeline::CalculateParametersDifference() NOEXCEPT
{
	static float yes{ 1.0f };

	float value = yes;
	yes *= 0.1f;

	return value;
}