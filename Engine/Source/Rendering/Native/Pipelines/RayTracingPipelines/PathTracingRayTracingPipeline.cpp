//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/PathTracingRayTracingPipeline.h>

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
void PathTracingRayTracingPipeline::Initialize() NOEXCEPT
{
	//Reset this ray tracing pipeline.
	ResetRayTracingPipeline();

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
	SetRayGenerationShader(ResourceSystem::Instance->GetShaderResource(HashString("PathTracingRayGenerationShader"))->_Handle);

	//Set the number of hit groups.
	SetNumberOfHitGroups(3);

	//Add the terrain hit group.
	AddHitGroup(ResourceSystem::Instance->GetShaderResource(HashString("TerrainRayClosestHitShader"))->_Handle, EMPTY_HANDLE, EMPTY_HANDLE);

	//Add the static models hit group.
	AddHitGroup(ResourceSystem::Instance->GetShaderResource(HashString("StaticModelRayClosestHitShader"))->_Handle, EMPTY_HANDLE, EMPTY_HANDLE);

	//Add the dynamic models hit group.
	AddHitGroup(ResourceSystem::Instance->GetShaderResource(HashString("DynamicModelRayClosestHitShader"))->_Handle, EMPTY_HANDLE, EMPTY_HANDLE);

	//Add the miss shaders.
	SetNumberOfMissShaders(2);
	AddMissShader(ResourceSystem::Instance->GetShaderResource(HashString("PathTracingRayMissShader"))->_Handle);
	AddMissShader(ResourceSystem::Instance->GetShaderResource(HashString("VisibilityRayMissShader"))->_Handle);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::RAY_GENERATION, 0, sizeof(PathTracingPushConstantData));

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the name.
	SetName("Path Tracing");
#endif

	//Finalize the pipeline.
	FinalizePipeline();
}

/*
*	Executes this ray tracing pipeline.
*/
void PathTracingRayTracingPipeline::Execute() NOEXCEPT
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
	command_buffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution(0)._Width, RenderingSystem::Instance->GetScaledResolution(0)._Height);

	//End the command buffer.
	command_buffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}

/*
*	Terminates this ray tracing pipeline.
*/
void PathTracingRayTracingPipeline::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);

	//Destroy the render data table layout.
	RenderingSystem::Instance->DestroyRenderDataTableLayout(&_RenderDataTableLayout);
}

/*
*	Creates the render data table layout.
*/
void PathTracingRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 5> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RAY_GENERATION)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void PathTracingRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_3));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(4, 0, &_RenderDataTable, RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_FEATURES_4));
}