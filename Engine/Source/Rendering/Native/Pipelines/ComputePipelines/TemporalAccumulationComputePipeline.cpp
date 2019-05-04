//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/TemporalAccumulationComputePipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TemporalAccumulationComputePipeline);

/*
*	Push constant data definition.
*/
class PushConstantData final
{

public:

	//Denotes whether or not temporal accumulation is enabled.
	int32 _Enabled;

};

/*
*	Default constructor.
*/
TemporalAccumulationComputePipeline::TemporalAccumulationComputePipeline() NOEXCEPT
{
	//Set the initialization function.
	SetInitializationFunction([]()
	{
		TemporalAccumulationComputePipeline::Instance->Initialize();
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		TemporalAccumulationComputePipeline::Instance->Execute();
	});
}

/*
*	Initializes this compute pipeline.
*/
void TemporalAccumulationComputePipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Set the shader.
	SetShader(Shader::TemporalAccumulationCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Compute, 0, sizeof(PushConstantData));
}

/*
*	Creates the render data table layout.
*/
void TemporalAccumulationComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 5> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Fragment),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Fragment)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void TemporalAccumulationComputePipeline::CreateRenderDataTables() NOEXCEPT
{
	_RenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &renderDataTable);

		RenderingSystem::Instance->BindStorageImageToRenderDataTable(1, 0, &renderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::Scene));
		RenderingSystem::Instance->BindStorageImageToRenderDataTable(2, 0, &renderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures1));
		RenderingSystem::Instance->BindStorageImageToRenderDataTable(3, 0, &renderDataTable, RenderingSystem::Instance->GetRenderTarget(RenderTarget::SceneFeatures3));
	}
}

/*
*	Executes this compute pipeline.
*/
void TemporalAccumulationComputePipeline::Execute() NOEXCEPT
{
	//Toggle enabled.
	static bool enabled{ false };

	if (ComponentManager::ReadSingletonComponent<InputComponent>()->_GamepadStates[0]._Y == ButtonState::Pressed)
	{
		enabled = !enabled;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Update the current render data table.
	RenderDataTableHandle &currentRenderDataTable{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &currentRenderDataTable, _CurrentBufferIndex == 0 ? RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1) : RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2));
	RenderingSystem::Instance->BindStorageImageToRenderDataTable(4, 0, &currentRenderDataTable, _CurrentBufferIndex == 0 ? RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer2) : RenderingSystem::Instance->GetRenderTarget(RenderTarget::TemporalAccumulationBuffer1));

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	commandBuffer->BindRenderDataTable(this, 1, currentRenderDataTable);

	//Push constants.
	PushConstantData data;

	data._Enabled = static_cast<int32>(enabled);

	commandBuffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(PushConstantData), &data);


	//Dispatch!
	commandBuffer->Dispatch(this, RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height, 1);

	//End the command buffer.
	commandBuffer->End(this);

	//Include in the final render.
	SetIncludeInRender(true);

	//Update the current buffer index.
	_CurrentBufferIndex = _CurrentBufferIndex == 0 ? 1 : 0;
}