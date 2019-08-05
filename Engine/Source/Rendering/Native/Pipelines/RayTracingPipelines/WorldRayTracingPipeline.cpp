//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetModelSystem()->GetModelDataRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the ray generation shader.
	SetRayGenerationShader(Shader::WorldRayGeneration);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::WorldRayClosestHit);

	//Add the miss shaders.
	SetNumberOfMissShaders(2);
	AddMissShader(Shader::WorldRayMiss);
	AddMissShader(Shader::VisibilityRayMiss);
}

/*
*	Executes this ray tracing pipeline.
*/
void WorldRayTracingPipeline::Execute() NOEXCEPT
{
	//No need to fire rays if there's nothing to fire against.
	const uint64 numberOfModelComponents{ ComponentManager::GetNumberOfModelComponents() };

	if (numberOfModelComponents == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Update the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(4, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(5, 0, &currentRenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures4));

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetModelSystem()->GetCurrentModelDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 3, currentRenderDataTable);

	//Trace rays!
	commandBuffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height);

	//End the command buffer.
	commandBuffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void WorldRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 6> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void WorldRayTracingPipeline::CreateRenderDataTables() NOEXCEPT
{
	_RenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);
	}
}