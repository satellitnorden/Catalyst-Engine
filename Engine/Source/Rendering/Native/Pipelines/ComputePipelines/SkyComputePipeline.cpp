//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/SkyComputePipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Initializes this compute pipeline.
*/
void SkyComputePipeline::Initialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data table.
	CreateRenderDataTable();

	//Set the shader.
	SetShader(Shader::SkyCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(_RenderDataTableLayout);
}

/*
*	Executes this compute pipeline.
*/
void SkyComputePipeline::Execute() NOEXCEPT
{
	//Cache data the will be used.
	CommandBuffer* const RESTRICT command_buffer{ GetCurrentCommandBuffer() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());
	command_buffer->BindRenderDataTable(this, 1, _RenderDataTable);

	//Dispatch!
	command_buffer->Dispatch(this, 32, 32, 6);

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}

/*
*	Creates the render data table layout.
*/
void SkyComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::Compute)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data table.
*/
void SkyComputePipeline::CreateRenderDataTable() NOEXCEPT
{
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &_RenderDataTable, WorldSystem::Instance->GetSkySystem()->GetSkyTexture());
}