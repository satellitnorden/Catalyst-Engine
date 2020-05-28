#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

class EditorSelectionSystem final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE EditorSelectionSystem() NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(EditorSelectionSystem));
	}

	/*
	*	Initializes the editor selection system.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Dynamic model selection data class definition.
	*/
	class DynamicModelSelectionData final
	{

	public:

		//Denotes if the user is currently selecting a model resource.
		bool _IsSelectingModelResource{ false };

	};

	//The currently selected entity.
	Entity *RESTRICT _CurrentlySelectedEntity{ nullptr };

	union
	{
		//The dynamic model selection data.
		DynamicModelSelectionData _DynamicModelSelectionData;
	};

	/*
	*	Sets the currently selected entity.
	*/
	void SetCurrentlySelectedEntity(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Updates the editor selection system during the PHYSICS update phase.
	*/
	void PhysicsUpdate() NOEXCEPT;

};
#endif
