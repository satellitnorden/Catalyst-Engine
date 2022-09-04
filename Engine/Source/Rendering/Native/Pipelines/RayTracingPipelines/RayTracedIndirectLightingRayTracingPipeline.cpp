//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/RayTracedIndirectLightingRayTracingPipeline.h>

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
*	Path tracing push constant data definition.
*/
class PathTracingPushConstantData final
{

public:

	//The number of samples.
	uint32 _NumberOfSamples;

};

/*
*	Initializes this ray tracing pipeline.
*/
void RayTracedIndirectLightingRayTracingPipeline::Initialize(const RenderingConfiguration::IndirectLightingQuality quality) NOEXCEPT
{
	//Reset this ray tracing pipeline.
	ResetRayTracingPipeline();

	//Set the indirect lighting quality.
	_IndirectLightingQuality = quality;

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the ray generation shader.
	SetRayGenerationShader(ResourceSystem::Instance->GetShaderResource(HashString("RayTracedIndirectLightingRayGenerationShader")));

	//Set the number of hit groups.
	SetNumberOfHitGroups(3);

	//Add the terrain hit group.
	AddHitGroup(ResourceSystem::Instance->GetShaderResource(HashString("TerrainRayClosestHitShader")), ResourcePointer<ShaderResource>(), ResourcePointer<ShaderResource>());

	//Add the static models hit group.
	AddHitGroup(ResourceSystem::Instance->GetShaderResource(HashString("StaticModelRayClosestHitShader")), ResourcePointer<ShaderResource>(), ResourcePointer<ShaderResource>());

	//Add the dynamic models hit group.
	AddHitGroup(ResourceSystem::Instance->GetShaderResource(HashString("DynamicModelRayClosestHitShader")), ResourcePointer<ShaderResource>(), ResourcePointer<ShaderResource>());

	//Add the miss shaders.
	SetNumberOfMissShaders(2);
	AddMissShader(ResourceSystem::Instance->GetShaderResource(HashString("PathTracingRayMissShader")));
	AddMissShader(ResourceSystem::Instance->GetShaderResource(HashString("VisibilityRayMissShader")));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::RAY_GENERATION, 0, sizeof(PathTracingPushConstantData));

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Ray Traced Indirect Lighting");
#endif
}

/*
*	Executes this ray tracing pipeline.
*/
void RayTracedIndirectLightingRayTracingPipeline::Execute() NOEXCEPT
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
	PathTracingPushConstantData data;

	data._NumberOfSamples = 1;

	command_buffer->PushConstants(this, ShaderStage::RAY_GENERATION, 0, sizeof(PathTracingPushConstantData), &data);

	//Trace rays!
	switch (_IndirectLightingQuality)
	{
		case RenderingConfiguration::IndirectLightingQuality::LOW:
		{
			command_buffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution(1)._Width, RenderingSystem::Instance->GetScaledResolution(1)._Height);

			break;
		}

		case RenderingConfiguration::IndirectLightingQuality::HIGH:
		{
			command_buffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution(0)._Width, RenderingSystem::Instance->GetScaledResolution(0)._Height);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}

/*
*	Terminates this ray tracing pipeline.
*/
void RayTracedIndirectLightingRayTracingPipeline::Terminate() NOEXCEPT
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
void RayTracedIndirectLightingRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 4> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void RayTracedIndirectLightingRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	switch (_IndirectLightingQuality)
	{
		case RenderingConfiguration::IndirectLightingQuality::LOW:
		{
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_HALF_1));
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_1_HALF));
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF));
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_3_HALF));

			break;
		}

		case RenderingConfiguration::IndirectLightingQuality::HIGH:
		{
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::INTERMEDIATE_RGBA_FLOAT32_1));
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));
			RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}