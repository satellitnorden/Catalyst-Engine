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
	*	Initializes the editor selection system.
	*/
	void Initialize() NOEXCEPT;

private:

	//The currently hovered entity.
	Entity *RESTRICT _CurrentlyHoveredEntity{ nullptr };

	//The currently selected entity.
	Entity *RESTRICT _CurrentlySelectedEntity{ nullptr };

	/*
	*	Updates the editor selection system during the PHYSICS update phase.
	*/
	void PhysicsUpdate() NOEXCEPT;

};
#endif
