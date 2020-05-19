//Header file.
#include <Rendering/Native/ModelSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes the model system.
*/
void ModelSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<ModelSystem *const RESTRICT>(arguments)->InputUpdate();
	},
	this,
	UpdatePhase::INPUT,
	UpdatePhase::USER_INTERFACE,
	false);
}

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
	StaticArray<RenderDataTableLayoutBinding, 4> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::RAY_CLOSEST_HIT)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_ModelDataRenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void ModelSystem::CreateRenderDataTables() NOEXCEPT
{
	//Create the compute render data tables.
	_ModelDataRenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &renderDataTable : _ModelDataRenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_ModelDataRenderDataTableLayout, &renderDataTable);
	}
}

/*
*	Updates the model system during the input update phase.
*/
void ModelSystem::InputUpdate() NOEXCEPT
{
	//Store the previous world transform for all dynamic model entities.
	{
		const uint64 number_of_dynamic_model_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		for (uint64 i{ 0 }; i < number_of_dynamic_model_components; ++i, ++component)
		{
			component->_PreviousWorldTransform = component->_CurrentWorldTransform;
		}
	}

	//Store the previous world transform for all animated model entities.
	{
		const uint64 number_of_animated_model_components{ ComponentManager::GetNumberOfAnimatedModelComponents() };
		AnimatedModelComponent *RESTRICT component{ ComponentManager::GetAnimatedModelAnimatedModelComponents() };

		for (uint64 i{ 0 }; i < number_of_animated_model_components; ++i, ++component)
		{
			component->_PreviousWorldTransform = component->_CurrentWorldTransform;
		}
	}
}