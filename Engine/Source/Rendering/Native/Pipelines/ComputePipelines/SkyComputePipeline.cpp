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

	//The sky light direction.
	Vector3<float> _SkyLightDirection;

	//Padding.
	Padding<4> _Padding1;

	//The sky light luminance.
	Vector3<float> _SkyLightLuminance;

	//Padding.
	Padding<4> _Padding2;

	//The resolution.
	uint32 _Resolution;

	//The current iteration.
	uint32 _CurrentIteration;

	//The parameters difference.
	float _ParametersDifference;

};

/*
*	Initializes this compute pipeline.
*/
void SkyComputePipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shader.
	SetShader(Shader::SkyCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

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

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	SkyPushConstantData data;

	//Pick the first directional light.
	data._SkyLightDirection = VectorConstants::UP;
	data._SkyLightLuminance = VectorConstants::ZERO;

	for (uint64 i{ 0 }, size{ ComponentManager::GetNumberOfLightComponents() }; i < size; ++i)
	{
		const LightComponent& component{ ComponentManager::GetLightLightComponents()[i] };

		if (static_cast<LightType>(component._LightType) == LightType::DIRECTIONAL)
		{
			data._SkyLightDirection = component._Direction;
			data._SkyLightLuminance = component._Luminance;

			break;
		}
	}

	data._Resolution = WorldSystem::Instance->GetSkySystem()->GetSkyTextureResolution();
	data._CurrentIteration = _CurrentIteration;
	data._ParametersDifference = CalculateParametersDifference();

	command_buffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(SkyPushConstantData), &data);

	//Dispatch!
	command_buffer->Dispatch(this, data._Resolution >> 1, data._Resolution >> 1, 6);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);

	//Update the current iteration.
	_CurrentIteration = (_CurrentIteration + 1) & 3;
}

/*
*	Creates the render data table layout.
*/
void SkyComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void SkyComputePipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, WorldSystem::Instance->GetSkySystem()->GetSkyTexture());
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