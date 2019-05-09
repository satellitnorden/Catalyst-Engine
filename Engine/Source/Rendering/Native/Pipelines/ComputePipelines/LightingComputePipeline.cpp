//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/LightingComputePipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/LightingSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes this compute pipeline.
*/
void LightingComputePipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shader.
	SetShader(Shader::LightingCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(3);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);
	AddRenderDataTableLayout(LightingSystem::Instance->GetLightingDataRenderDataTableLayout());
}

/*
*	Executes this compute pipeline.
*/
void LightingComputePipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);
	commandBuffer->BindRenderDataTable(this, 2, LightingSystem::Instance->GetCurrentLightingDataRenderDataTable());

	//Dispatch!
	commandBuffer->Dispatch(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void LightingComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 6> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void LightingComputePipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::DiffuseIrradiance));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SpecularIrradiance));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(4, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(5, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));
}