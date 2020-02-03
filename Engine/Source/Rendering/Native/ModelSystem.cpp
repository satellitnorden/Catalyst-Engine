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
}

/*
*	Updates the model system during the pre update phase.
*/
void ModelSystem::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Store the previous world transform for all model entities.
	const uint64 numberOfModelComponents{ ComponentManager::GetNumberOfModelComponents() };
	ModelComponent *RESTRICT modelComponent{ ComponentManager::GetModelModelComponents() };

	for (uint64 i{ 0 }; i < numberOfModelComponents; ++i, ++modelComponent)
	{
		modelComponent->_PreviousWorldTransform = modelComponent->_CurrentWorldTransform;
	}

	//Store the previous world transform for all animated model entities.
	const uint64 numberOfAnimatedModelComponents{ ComponentManager::GetNumberOfAnimatedModelComponents() };
	AnimatedModelComponent *RESTRICT animatedModelComponent{ ComponentManager::GetAnimatedModelAnimatedModelComponents() };

	for (uint64 i{ 0 }; i < numberOfAnimatedModelComponents; ++i, ++animatedModelComponent)
	{
		animatedModelComponent->_PreviousWorldTransform = animatedModelComponent->_CurrentWorldTransform;
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
*	Enables highlight on a model entity.
*/
void ModelSystem::EnableHighlight(const ModelEntity* const RESTRICT entity, const Vector3<float>& color, const float strength) NOEXCEPT
{
	ASSERT(entity->_Initialized, "Model entity is not initialized yet - cannot enable highlight!");

	_HighlightedModels.Emplace(color, entity->_ComponentsIndex, strength);
}

/*
*	Sets the highlight color on a model entity.
*/
void ModelSystem::SetHighlightColor(const ModelEntity *const RESTRICT entity, const Vector3<float> &color) NOEXCEPT
{
	ASSERT(entity->_Initialized, "Model entity is not initialized yet - cannot set highlight color!");

	for (uint64 i{ 0 }, size{ _HighlightedModels.Size() }; i < size; ++i)
	{
		if (_HighlightedModels[i]._ComponentsIndex == entity->_ComponentsIndex)
		{
			_HighlightedModels[i]._HighlightColor = color;

			break;
		}
	}
}

/*
*	Disables highlight on a model entity.
*/
void ModelSystem::DisableHighlight(const ModelEntity* const RESTRICT entity) NOEXCEPT
{
	ASSERT(entity->_Initialized, "Model entity is not initialized yet - cannot disable highlight!");

	for (uint64 i{ 0 }, size{ _HighlightedModels.Size() }; i < size; ++i)
	{
		if (_HighlightedModels[i]._ComponentsIndex == entity->_ComponentsIndex)
		{
			_HighlightedModels.EraseAt(i);

			break;
		}
	}
}

/*
*	Creates the render data table layout.
*/
void ModelSystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Create the model data render data table layout.
	StaticArray<RenderDataTableLayoutBinding, 4> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayGeneration | ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageBuffer, RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageBuffer, RenderingConstants::MAXIMUM_NUMBER_OF_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::RayClosestHit)
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