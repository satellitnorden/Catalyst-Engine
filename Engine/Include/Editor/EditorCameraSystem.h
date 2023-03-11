#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Gameplay.
#include <Gameplay/FreeflyCamera.h>

class EditorCameraSystem final
{

public:

	/*
	*	Initializes the editor camera system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the editor camera system.
	*/
	void Update() NOEXCEPT;

private:

	//The freefly camera.
	FreeflyCamera _FreeflyCamera;

};

#endif
