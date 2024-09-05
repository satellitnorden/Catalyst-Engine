#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Generated.
#include <Generated/UserInterface.Generated.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//Forward declarations.
class RenderInputStream;

class UserInterfaceInitializationData final : public ComponentInitializationData
{

public:

	//The identifier.
	UserInterfaceSceneIdentifier _Identifier;

};

class UserInterfaceInstanceData final
{

public:

	//The identifier.
	UserInterfaceSceneIdentifier _Identifier;

	//The scene.
	UserInterfaceScene *RESTRICT _Scene;

};

class UserInterfaceComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		UserInterface,
		COMPONENT_INITIALIZE()
		COMPONENT_POST_CREATE_INSTANCE()
	);

private:

	/*
	*	Gathers the user interface 3D background input stream.
	*/
	void GatherUserInterface3DBackgroundInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

};