//Header file.
#include <Rendering/Native/Pipelines/RayTracingPipelines/VisibilityRayTracingPipeline.h>

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
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The light index.
	int32 _LightIndex;

};

/*
*	Initializes this ray tracing pipeline.
*/
void VisibilityRayTracingPipeline::Initialize(const int32 lightIndex) NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Store the light index.
	_LightIndex = lightIndex;

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(4);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetModelSystem()->GetModelDataRenderDataTableLayout());
	AddRenderDataTableLayout(RenderingSystem::Instance->GetLightingSystem()->GetLightingDataRenderDataTableLayout());
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::RayGeneration, 0, sizeof(PushConstantData));

	//Set the ray generation shader.
	SetRayGenerationShader(Shader::VisibilityRayGeneration);

	//Set the closest hit shader.
	SetClosestHitShader(Shader::None);

	//Add the miss shaders.
	SetNumberOfMissShaders(1);
	AddMissShader(Shader::VisibilityRayMiss);
}

/*
*	Executes this ray tracing pipeline.
*/
void VisibilityRayTracingPipeline::Execute() NOEXCEPT
{
	//No need to fire rays if there's nothing to fire against.
	const uint64 numberOfStaticModelComponents{ ComponentManager::GetNumberOfStaticModelComponents() };

	if (numberOfStaticModelComponents == 0)
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
	commandBuffer->BindRenderDataTable(this, 1, RenderingSystem::Instance->GetModelSystem()->GetCurrentModelDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 2, RenderingSystem::Instance->GetLightingSystem()->GetCurrentLightingDataRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 3, _RenderDataTable);

	//Push constants.
	PushConstantData data;

	data._LightIndex = _LightIndex;

	commandBuffer->PushConstants(this, ShaderStage::RayGeneration, 0, sizeof(PushConstantData), &data);

	//Trace rays!
	commandBuffer->TraceRays(this, RenderingSystem::Instance->GetScaledResolution()._Width / 2, RenderingSystem::Instance->GetScaledResolution()._Height / 2);

	//End the command buffer.
	commandBuffer->End(this);

	//Include in render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void VisibilityRayTracingPipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::CombinedImageSampler, 1, ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::RayGeneration)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void VisibilityRayTracingPipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2), RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeNearest_AddressModeClampToEdge));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate_Half_R8_Byte_1));
}