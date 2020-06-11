#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/EntityCore.h>

//Math.
#include <Math/General/Vector.h>

class EditorPlacementSystem final
{

public:

	/*
	*	Updates the editor placement system.
	*/
	void Update() NOEXCEPT;

private:

	/*
	*	Placement data class definition.
	*/
	class PlacementData final
	{

	public:

		//The entity type.
		EntityType _EntityType{ EntityType::DynamicModel };

		//The number of instances.
		uint32 _NumberOfInstances{ 0 };

	};

	//The minimum.
	Vector3<float32> _Minimum{ -64.0f, 0.0f, -64.0f };

	//The maximum.
	Vector3<float32> _Maximum{ 64.0f, 0.0f, 64.0f };

	//The placement data.
	DynamicArray<PlacementData> _PlacementData;

	//The currently selected placement data index.
	uint64 _CurrentlySelectedPlacementDataIndex{ UINT64_MAXIMUM };

	//The place entities.
	DynamicArray<Entity *RESTRICT> _PlacedEntities;

	/*
	*	Adds the currently selected placement data window.
	*/
	void AddCurrentlySelectedPlacementDataWindow() NOEXCEPT;

	/*
	*	Places.
	*/
	void Place() NOEXCEPT;

	/*
	*	Undoes.
	*/
	void Undo() NOEXCEPT;

};

#endif
