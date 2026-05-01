#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Forward declarations.
class RenderInputStream;

class UserInterfaceInitializationData final : public ComponentInitializationData
{

public:

	//The identifier.
#if 0
	UserInterfaceSceneIdentifier _Identifier;
#endif

};

class UserInterfaceInstanceData final
{

public:

	//The identifier.
#if 0
	UserInterfaceSceneIdentifier _Identifier;
#endif

	//The scene.
#if 0
	UserInterfaceScene *RESTRICT _Scene;
#endif

};

class UserInterfaceComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		UserInterface,
		COMPONENT_INITIALIZE()
		COMPONENT_POST_CREATE_INSTANCE()
		COMPONENT_SELECT()
	);

private:

	/*
	*	Gathers the user interface 3D background input stream.
	*/
	void GatherUserInterface3DBackgroundInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

};