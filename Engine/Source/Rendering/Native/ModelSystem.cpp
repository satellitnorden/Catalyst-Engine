//Header file.
#include <Rendering/Native/ModelSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post-initializes the model system.
*/
void ModelSystem::PostInitialize() NOEXCEPT
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	CreateRenderDataTables();

	//Create the uniform buffers.
	CreateUniformBuffers();
}

/*
*	Updates the lighting system.
*/
void ModelSystem::Update(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the current model data render data table.
	RenderDataTableHandle &currentModelDataRenderDataTable{ _ModelDataRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	const uint64 numberOfStaticModelComponents{ ComponentManager::GetNumberOfStaticModelComponents() };
	const StaticModelComponent *RESTRICT staticModelComponent{ ComponentManager::GetStaticModelStaticModelComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetStaticModelTransformComponents() };

	ASSERT(numberOfStaticModelComponents < RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, "Increase maximum number of models plz. c:");

	_TopLevelAccelerationStructureInstances.ClearFast();
	StaticArray<Material, RenderingConstants::MAXIMUM_NUMBER_OF_MODELS> materials;

	for (uint64 i{ 0 }; i < numberOfStaticModelComponents; ++i, ++staticModelComponent, ++transformComponent)
	{
		_TopLevelAccelerationStructureInstances.EmplaceSlow(transformComponent->_WorldTransform, staticModelComponent->_Model->_BottomLevelAccelerationStructure, i);

		RenderingSystem::Instance->BindStorageBufferToRenderDataTable(0, static_cast<uint32>(i), &currentModelDataRenderDataTable, staticModelComponent->_Model->_VertexBuffer);
		RenderingSystem::Instance->BindStorageBufferToRenderDataTable(1, static_cast<uint32>(i), &currentModelDataRenderDataTable, staticModelComponent->_Model->_IndexBuffer);

		materials[i] = staticModelComponent->_Material;
	}

	BufferHandle &currentMaterialsUniformBuffer{ _MaterialsUniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	const void *const RESTRICT dataChunks[]{ materials.Data() };
	const uint64 dataSizes[]{ sizeof(Material) * RenderingConstants::MAXIMUM_NUMBER_OF_MODELS };

	RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &currentMaterialsUniformBuffer);

	RenderingSystem::Instance->BindUniformBufferToRenderDataTable(2, 0, &currentModelDataRenderDataTable, currentMaterialsUniformBuffer);

	//Re-create the top level acceleration structure.
	if (_TopLevelAccelerationStructure)
	{
		RenderingSystem::Instance->DestroyTopLevelAccelerationStructure(&_TopLevelAccelerationStructure);
	}

	if (!_TopLevelAccelerationStructureInstances.Empty())
	{
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(RenderingSystem::Instance->GetModelSystem()->GetTopLevelAccelerationStructureInstances()), &_TopLevelAccelerationStructure);
	}
}

/*
*	Returns the current model data render data table.
*/
RenderDataTableHandle ModelSystem::GetCurrentModelDataRenderDataTable() const NOEXCEPT
{
	//Return the current lighting data compute render data table.
	return _ModelDataRenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Creates the render data table layout.
*/
void ModelSystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Create the model data render data table layout.
	StaticArray<RenderDataTableLayoutBinding, 3> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::StorageBuffer, RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageBuffer, RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::RayClosestHit)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_ModelDataRenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void ModelSystem::CreateRenderDataTables() NOEXCEPT
{
	//Create the compute render data tables.
	_ModelDataRenderDataTables.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _ModelDataRenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_ModelDataRenderDataTableLayout, &renderDataTable);
	}
}

/*
*	Creates the uniform buffers.
*/
void ModelSystem::CreateUniformBuffers() NOEXCEPT
{
	//Create the uniform buffers.
	_MaterialsUniformBuffers.UpsizeFast(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (BufferHandle &materialsUniformBuffer : _MaterialsUniformBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(Material) * RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &materialsUniformBuffer);
	}
}