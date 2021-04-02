//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/TerrainHeightGenerationComputePipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Terrain height generation push constant data definition.
*/
class TerrainHeightGenerationPushConstantData final
{

public:

	//The minimum bounds.
	Vector2<float32> _MinimumBounds;

	//The maximum bounds.
	Vector2<float32> _MaximumBounds;

	//The dimensions
	uint32 _Dimensions;

};

/*
*	Initializes this compute pipeline.
*/
void TerrainHeightGenerationComputePipeline::Initialize(const ResourcePointer<ShaderResource> shader) NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Set the shader.
	SetShader(shader);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::GLOBAL));
	AddRenderDataTableLayout(_RenderDataTableLayout);

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::COMPUTE, 0, sizeof(TerrainHeightGenerationPushConstantData));

	//This pipeline is now initialized!
	_Initialized = true;
}

/*
*	Executes this compute pipeline.
*/
void TerrainHeightGenerationComputePipeline::Execute(	const Vector2<float32> minimum_bounds,
														const Vector2<float32> maximum_bounds,
														const uint32 dimensions,
														const Texture2DHandle height_texture,
														CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//Create a new render data table for the texture(s).
	RenderDataTableHandle render_data_table;
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);

	RenderingSystem::Instance->BindStorageImageToRenderDataTable(0, 0, &render_data_table, height_texture);

	_RenderDataTables.Emplace(render_data_table);

	//Bind the pipeline.
	command_buffer->BindPipeline(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 1, render_data_table);

	//Push constants.
	TerrainHeightGenerationPushConstantData data;

	data._MinimumBounds = minimum_bounds;
	data._MaximumBounds = maximum_bounds;
	data._Dimensions = dimensions;

	command_buffer->PushConstants(this, ShaderStage::COMPUTE, 0, sizeof(TerrainHeightGenerationPushConstantData), &data);

	//Dispatch!
	command_buffer->Dispatch(this, dimensions, dimensions, 1);
}

/*
*	Destroys the render data tables.
*/
void TerrainHeightGenerationComputePipeline::DestroyRenderDataTables() NOEXCEPT
{
	for (RenderDataTableHandle render_data_table : _RenderDataTables)
	{
		RenderingSystem::Instance->DestroyRenderDataTable(&render_data_table);
	}

	_RenderDataTables.Clear();
}

/*
*	Creates the render data table layout.
*/
void TerrainHeightGenerationComputePipeline::CreateRenderDataTableLayout() NOEXCEPT
{
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageImage, 1, ShaderStage::COMPUTE)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}