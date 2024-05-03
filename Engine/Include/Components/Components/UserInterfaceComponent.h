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
	);

public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

private:

	/*
	*	Gathers the user interface 3D background input stream.
	*/
	void GatherUserInterface3DBackgroundInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT;

};