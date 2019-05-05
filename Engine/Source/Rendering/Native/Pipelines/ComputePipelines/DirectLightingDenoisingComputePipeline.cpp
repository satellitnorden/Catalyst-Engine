//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/DirectLightingDenoisingComputePipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/LightingSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectLightingDenoisingComputePipeline);

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//The index.
	int32 _Index;

	//The pass.
	int32 _Pass;

};

/*
*	Default constructor.
*/
DirectLightingDenoisingComputePipeline::DirectLightingDenoisingComputePipeline() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([]()
	{
		DirectLightingDenoisingComputePipeline::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		DirectLightingDenoisingComputePipeline::Instance->Execute();
	});
}

/*
*	Initializes this compute pipeline.
*/
void DirectLightingDenoisingComputePipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shader.
	SetShader(Shader::DirectLightingDenoisingCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(3);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);
	AddRenderDataTableLayout(LightingSystem::Instance->GetLightingDataRenderDataTableLayout());

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Compute, 0, sizeof(PushConstantData));
}

/*
*	Creates the render data table layout.
*/
void DirectLightingDenoisingComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 2> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void DirectLightingDenoisingComputePipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &_RenderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Intermediate));
}

/*
*	Executes this compute pipeline.
*/
void DirectLightingDenoisingComputePipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, _RenderDataTable);
	commandBuffer->BindRenderDataTable(this, 2, LightingSystem::Instance->GetCurrentLightingDataRenderDataTable());

	//Denoise the direct lighting result of the directional light.
	{
		//Push constants.
		PushConstantData data;

		data._Index = 0;
		data._Pass = 0;

		commandBuffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(PushConstantData), &data);

		//Dispatch!
		commandBuffer->Dispatch(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height, 1);
	}

	{
		//Push constants.
		PushConstantData data;

		data._Index = 0;
		data._Pass = 1;

		commandBuffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(PushConstantData), &data);

		//Dispatch!
		commandBuffer->Dispatch(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height, 1);
	}

	//Denoise all lights.
	for (uint64 i{ 0 }, size{ ComponentManager::GetNumberOfLightComponents() }; i < size; ++i)
	{
		{
			//Push constants.
			PushConstantData data;

			data._Index = static_cast<int32>(i + 1);
			data._Pass = 0;

			commandBuffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(PushConstantData), &data);

			//Dispatch!
			commandBuffer->Dispatch(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height, 1);
		}

		{
			//Push constants.
			PushConstantData data;

			data._Index = static_cast<int32>(i + 1);
			data._Pass = 1;

			commandBuffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(PushConstantData), &data);

			//Dispatch!
			commandBuffer->Dispatch(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height, 1);
		}
	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include in the final render.
	SetIncludeInRender(true);
}