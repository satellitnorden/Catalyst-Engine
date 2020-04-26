//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/SkyComputePipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Sky push constant data definition.
*/
class SkyPushConstantData final
{

public:

	//The upper sky color.
	Vector3<float32> _UpperSkyColor;

	//Padding.
	Padding<4> _Padding1;

	//The lower sky color.
	Vector3<float32> _LowerSkyColor;

	//Padding.
	Padding<4> _Padding2;

	//The sky light direction.
	Vector3<float32> _SkyLightDirection;

	//Padding.
	Padding<4> _Padding3;

	//The sky light luminance.
	Vector3<float32> _SkyLightLuminance;

	//Padding.
	Padding<4> _Padding4;

	//The sky light intensity.
	float32 _SkyLightIntensity;

	//The current face index.
	uint32 _CurrentFaceIndex;

	//The star intensity.
	float32 _StarIntensity;

	//Denotes if there is a sky texture override.
	uint32 _HasSkyTextureOverride;

	//The sky intensity.
	float32 _SkyIntensity;

};

/*
*	Initializes this compute pipeline.
*/
void SkyComputePipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Set the shader.
	SetShader(ResourceSystem::Instance->GetShaderResource(HashString("SkyComputeShader")));

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::COMPUTE, 0, sizeof(SkyPushConstantData));
}

/*
*	Executes this compute pipeline.
*/
void SkyComputePipeline::Execute() NOEXCEPT
{
	//Create the render data table, if necessary
	CreateRenderDataTable();

	//Cache data the will be used.
	CommandBuffer* const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Push constants.
	SkyPushConstantData data;

	const SkyGradient current_sky_gradient{ WorldSystem::Instance->GetSkySystem()->GetCurrentSkyGradient() };
	data._UpperSkyColor = current_sky_gradient._UpperSkyColor;
	data._LowerSkyColor = current_sky_gradient._LowerSkyColor;

	//Pick the first directional light.
	data._SkyLightDirection = CatalystWorldCoordinateSpace::UP;
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

	data._CurrentFaceIndex = _CurrentFaceIndex;
	data._StarIntensity = WorldSystem::Instance->GetSkySystem()->GetCurrentStarIntensity();
	data._HasSkyTextureOverride = WorldSystem::Instance->GetSkySystem()->GetSkyTextureOverride() ? 1 : 0;
	data._SkyIntensity = WorldSystem::Instance->GetSkySystem()->GetSkyIntensity();

	command_buffer->PushConstants(this, ShaderStage::COMPUTE, 0, sizeof(SkyPushConstantData), &data);

	//Dispatch!
	command_buffer->Dispatch(this, CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION, CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION, 1);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);

	//Update the current face index.
	_CurrentFaceIndex = (_CurrentFaceIndex + 1) % 6;
}

/*
*	Creates the render data table layout.
*/
void SkyComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::COMPUTE)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void SkyComputePipeline::CreateRenderDataTable() NOEXCEPT
{
	//Determine the texture that SHOULD be bound.
	TextureCubeHandle sky_texture_that_should_be_bound;

	const Optional<TextureCubeHandle> sky_texture_override{ WorldSystem::Instance->GetSkySystem()->GetSkyTextureOverride() };

	if (sky_texture_override)
	{
		sky_texture_that_should_be_bound = sky_texture_override;
	}

	else
	{
		sky_texture_that_should_be_bound = WorldSystem::Instance->GetSkySystem()->GetSkyTexture(0);
	}

	if (_CurrentlyBoundSkyTexture != sky_texture_that_should_be_bound)
	{
		//Destroy the old render data table, if necessary.
		if (_RenderDataTable)
		{
			RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);
		}

		//Create the render data table.
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

		//Bind the sky texture.
		RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, sky_texture_that_should_be_bound, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
	
		//Remember which sky texture is currently bound.
		_CurrentlyBoundSkyTexture = sky_texture_that_should_be_bound;
	}
}