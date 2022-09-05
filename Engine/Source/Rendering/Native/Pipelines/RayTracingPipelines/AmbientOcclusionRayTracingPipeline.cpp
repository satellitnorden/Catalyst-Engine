//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/AmbientOcclusionRayTracingPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Ray traced ambient occlusion push constant data definition.
*/
class RayTracedAmbientOcclusionPushConstantData final
{

public:

	//The ambient occlusion number of samples.
	uint32 _AmbientOcclusionNumberOfSamples;

};

/*
*	Initializes this ray tracing pipeline.
*/
void AmbientOcclusionRayTracingPipeline::Initialize(const RenderTargetHandle ambient_occlusion_render_target) NOEXCEPT
{
	//Reset this ray tracing pipeline.
	ResetRayTracingPipeline();

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable(ambient_occlusion_render_target);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::RAY_GENERATION, 0, sizeof(RayTracedAmbientOcclusionPushConstantData));

	//Set the ray generation shader.
	SetRayGenerationShader(ResourceSystem::Instance->GetShaderResource(HashString("RayTracedAmbientOcclusionRayGenerationShader")));

	//Add the miss shaders.
	SetNumberOfMissShaders(1);
	AddMissShader(ResourceSystem::Instance->GetShaderResource(HashString("VisibilityRayMissShader")));

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Ambient Occlusion");
#endif
}

/*
*	Executes this ray tracing pipeline.
*/
void AmbientOcclusionRayTracingPipeline::Execute() NOEXCEPT
{
	//Check if ray tracing is possible.
	if (!RenderingSystem::Instance->IsRayTracingPossible())
	{
		SetIncludeInRender(false);

		return;
	}

	//Retrieve and set the command buffer.
	CommandBuffer *const RESTRICT command_buffer{ RenderingSystem::Instance->GetGlobalCommandBuffer(CommandBufferLevel::SECONDARY) };
	SetCommandBuffer(command_buffer);

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTable());
	command_buffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());
	command_buffer->BindRenderDataTable(this, 3, _RenderDataTable);

	//Push constants.
	RayTracedAmbientOcclusionPushConstantData data;

	switch (RenderingSystem::Instance->GetRenderingConfiguration()->GetAmbientOcclusionQuality())
	{
		case RenderingConfiguration::AmbientOcclusionQuality::LOW:
		{
			data._AmbientOcclusionNumberOfSamples = 4;

			break;
		}

		case RenderingConfiguration::AmbientOcclusionQuality::MEDIUM:
		{
			data._AmbientOcclusionNumberOfSamples = 8;

			break;
		}

		case RenderingConfiguration::AmbientOcclusionQuality::HIGH:
		{
			data._AmbientOcclusionNumberOfSamples = 16;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	command_buffer->PushConstants(this, ShaderStage::RAY_GENERATION, 0, sizeof(RayTracedAmbientOcclusionPushConstantData), &data);

	//Trace rays!
	command_buffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution(1)._Width, RenderingSystem::Instance->GetScaledResolution(1)._Height);

	//End the command buffer.
	command_buffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}

/*
*	Terminates this ray tracing pipeline.
*/
void AmbientOcclusionRayTracingPipeline::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);

	//Destroy the render data table layout.
	RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
}

/*
*	Creates the render data table layout.
*/
void AmbientOcclusionRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void AmbientOcclusionRayTracingPipeline::CreateRenderDataTable(const RenderTargetHandle ambient_occlusion_render_target) NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2_HALF));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, ambient_occlusion_render_target);
}