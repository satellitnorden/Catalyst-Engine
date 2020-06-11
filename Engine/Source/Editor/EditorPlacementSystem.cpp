#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorPlacementSystem.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>
#include <Entities/Types/DynamicModelEntity.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Noise/HammersleySequence.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor placement system.
*/
void EditorPlacementSystem::Update() NOEXCEPT
{
	//Is the current contextual window PLACEMENT?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::PLACEMENT)
	{
		return;
	}

	//Add the placement window.
	ImGui::Begin("Placement", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the widgets for the minimum/maximum.
	ImGui::DragFloat3("Minimum", _Minimum.Data());
	ImGui::DragFloat3("Maximum", _Maximum.Data());

	//Add buttons for all placement data.
	for (uint64 i{ 0 }, size{ _PlacementData.Size() }; i < size; ++i)
	{
		char buffer[32];
		sprintf_s(buffer, "Placement Data #%llu", i);

		if (ImGui::Button(buffer))
		{
			_CurrentlySelectedPlacementDataIndex = i;

			break;
		}
	}

	//Adds the button for adding new placement data.
	if (ImGui::Button("Add Placement Data"))
	{
		_PlacementData.Emplace();
	}

	//Add the currently selected placement data window.
	AddCurrentlySelectedPlacementDataWindow();

	//Add the "Place!" button.
	if (ImGui::Button("Place!"))
	{
		Place();
	}

	//Add the "Undo" button.
	if (!_PlacedEntities.Empty())
	{
		if (ImGui::Button("Undo"))
		{
			Undo();
		}
	}

	ImGui::End();
}

/*
*	Adds the currently selected placement data window.
*/
void EditorPlacementSystem::AddCurrentlySelectedPlacementDataWindow() NOEXCEPT
{
	//Has the user selected placement data?
	if (_CurrentlySelectedPlacementDataIndex >= _PlacementData.Size())
	{
		return;
	}

	//Cache the placement data.
	PlacementData &placement_data{ _PlacementData[_CurrentlySelectedPlacementDataIndex] };

	//Add the "Currently Selected Placement Data" window.
	ImGui::Begin("Currently Selected Placement Data", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(512.0f, 512.0f));

	//Add the widget for modifying the number of instances.
	ImGui::DragInt("Number Of Instances", reinterpret_cast<int32 *const RESTRICT>(&placement_data._NumberOfInstances), 0);

	ImGui::End();
}

/*
*	Places.
*/
void EditorPlacementSystem::Place() NOEXCEPT
{
	for (const PlacementData &placement_data : _PlacementData)
	{
		for (uint32 i{ 0 }; i < placement_data._NumberOfInstances; ++i)
		{
			//Calculate the position.
			const Vector3<float32> position{	CatalystRandomMath::RandomFloatInRange(_Minimum._X, _Maximum._X),
				CatalystRandomMath::RandomFloatInRange(_Minimum._Y, _Maximum._Y),
				CatalystRandomMath::RandomFloatInRange(_Minimum._Z, _Maximum._Z) };

			//Create the entity.
			{
				DynamicModelEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<DynamicModelEntity>() };
				DynamicModelInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicModelInitializationData>() };

				data->_Properties = EntityInitializationData::Property::NONE;
				data->_InitialWorldTransform = WorldTransform(position, VectorConstants::ZERO, 1.0f);
				data->_ModelResource = ResourceSystem::Instance->GetModelResource(HashString("Catalyst_Engine_Default_Model"));

				for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
				{
					data->_MaterialResources[i] = ResourceSystem::Instance->GetMaterialResource(HashString("Catalyst_Engine_Default_Material"));
				}

				data->_ModelCollisionConfiguration._Type = ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX;
				data->_SimulatePhysics = false;

				EntitySystem::Instance->RequestInitialization(entity, data, true);

				_PlacedEntities.Emplace(entity);
			}
		}
	}
}

/*
*	Undoes.
*/
void EditorPlacementSystem::Undo() NOEXCEPT
{
	for (Entity *const RESTRICT place_entity : _PlacedEntities)
	{
		EntitySystem::Instance->RequestTermination(place_entity);
		EntitySystem::Instance->RequestDestruction(place_entity);
	}

	_PlacedEntities.Clear();
}
#endif