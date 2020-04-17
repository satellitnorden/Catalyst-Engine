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

/*
*	Initializes this ray tracing pipeline.
*/
void AmbientOcclusionRayTracingPipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Set the ray generation shader.
	SetRayGenerationShader(Shader::RayTracedAmbientOcclusionRayGeneration);

	//Add the miss shaders.
	SetNumberOfMissShaders(1);
	AddMissShader(Shader::VisibilityRayMiss);
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

	//Cache data the will be used.
	CommandBuffer *const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTable());
	command_buffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());
	command_buffer->BindRenderDataTable(this, 3, _RenderDataTable);

	//Trace rays!
	command_buffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution(1)._Width, RenderingSystem::Instance->GetScaledResolution(1)._Height);

	//End the command buffer.
	command_buffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
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
void AmbientOcclusionRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SCENE_FEATURES_2_HALF));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::AMBIENT_OCCLUSION));
}