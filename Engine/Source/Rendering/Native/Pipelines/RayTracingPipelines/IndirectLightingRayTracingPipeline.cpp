//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/IndirectLightingRayTracingPipeline.h>

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
void IndirectLightingRayTracingPipeline::Initialize() NOEXCEPT
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
	SetRayGenerationShader(Shader::IndirectLightingRayGeneration);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::None);

	//Add the miss shaders.
	SetNumberOfMissShaders(1);
	AddMissShader(Shader::VisibilityRayMiss);
}

/*
*	Executes this ray tracing pipeline.
*/
void IndirectLightingRayTracingPipeline::Execute() NOEXCEPT
{
	//No need to fire rays if there's nothing to fire against.
	const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };

	if (number_of_components == 0)
	{
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetRayTracingSystem()->GetRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 3, _RenderDataTable);

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
void IndirectLightingRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 3> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void IndirectLightingRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_R32G32B32A32_Float_1));
}