#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Rendering/Native/RenderPasses/ImGuiRenderPass.h>

//Rendering.
#include <Rendering/Native/NativeRenderPassManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ImGuiRenderPass::ImGuiRenderPass() NOEXCEPT
{
	//Register this render pass.
	NativeRenderPassManager::RegisterDefaultRenderPass(DefaultNativeRenderPassStage::IMGUI, this);

	//Set the initialization function.
	SetInitializationFunction([]()
	{
		ImGuiRenderPass::Instance->Initialize();
	});

	//Set the pre-record function.
	SetPreRecordFunction([](CommandBuffer *const RESTRICT frame_command_buffer)
	{
		ImGuiRenderPass::Instance->PreRecord(frame_command_buffer);
	});

	//Set the execution function.
	SetExecutionFunction([]()
	{
		ImGuiRenderPass::Instance->Execute();
	});

	//Set the termination function function.
	SetTerminationFunction([]()
	{
		ImGuiRenderPass::Instance->Terminate();
	});
}

/*
*	Initializes this render pass.
*/
void ImGuiRenderPass::Initialize() NOEXCEPT
{	
	//Reset this render pass.
	ResetRenderPass();

	//Register the input stream(s).
	{
		struct ImGuiPushConstantData final
		{
			//The scale.
			Vector2<float32> _Scale;

			//The translation.
			Vector2<float32> _Translation;
		};

		DynamicArray<VertexInputAttributeDescription> vertex_input_attribute_descriptions;

		vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, 0);
		vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, sizeof(Vector2<float32>));
		vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat, sizeof(Vector2<float32>) + sizeof(Vector2<float32>));

		DynamicArray<VertexInputBindingDescription> vertex_input_binding_descriptions;

		vertex_input_binding_descriptions.Emplace(0, sizeof(Vector2<float32>) + sizeof(Vector2<float32>) + sizeof(Vector4<float32>), VertexInputBindingDescription::InputRate::Vertex);


		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("ImGui"),
			vertex_input_attribute_descriptions,
			vertex_input_binding_descriptions,
			sizeof(ImGuiPushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				//Clear the entries.
				input_stream->_Entries.Clear();

				//Clear the push constant data memory.
				input_stream->_PushConstantDataMemory.Clear();
			},
			RenderInputStream::Mode::DRAW,
			nullptr
		);
	}

	//Add the pipelines.
	SetNumberOfPipelines(1);
	AddPipeline(&_ImGuiPipeline);

	//Initialize all pipelines.
	_ImGuiPipeline.Initialize();
}

/*
*	Pre-records this render pass.
*/
void ImGuiRenderPass::PreRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT
{
#if defined(CATALYST_EDITOR)
	frame_command_buffer->BlitImage
	(
		&_ImGuiPipeline,
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::SCENE_LOW_DYNAMIC_RANGE_1),
		RenderingSystem::Instance->GetSharedRenderTargetManager()->GetSharedRenderTarget(SharedRenderTarget::EDITOR_VIEWPORT)
	);
#endif

	//Bind the pipeline.
	frame_command_buffer->BindPipeline(&_ImGuiPipeline);

	//Bind the render data table.
	frame_command_buffer->BindRenderDataTable(&_ImGuiPipeline, 0, RenderingSystem::Instance->GetGlobalRenderDataTable2());

	//Draw ImGui.
	frame_command_buffer->DrawImGui(&_ImGuiPipeline);

	//Don't include the pipeline in the render since it's manually included here.
	_ImGuiPipeline.SetIncludeInRender(false);
}

/*
*	Executes this render pass.
*/
void ImGuiRenderPass::Execute() NOEXCEPT
{

}

/*
*	Terminates this render pass.
*/
void ImGuiRenderPass::Terminate() NOEXCEPT
{
	//Terminate all pipelines.
	_ImGuiPipeline.Terminate();
}
#endif