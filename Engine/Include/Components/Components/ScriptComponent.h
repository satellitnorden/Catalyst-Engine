#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Generated
#include <Generated/Script.Generated.h>

class ScriptInitializationData final : public ComponentInitializationData
{

public:

	//The script identifier.
	ScriptIdentifier _ScriptIdentifier;

};

class ScriptInstanceData final
{

public:

	//The script identifier.
	ScriptIdentifier _ScriptIdentifier;

	//The data.
	void *RESTRICT _Data;

};

class ScriptComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		Script,
		COMPONENT_INITIALIZE()
		COMPONENT_SERIAL_UPDATE(UpdatePhase::GAMEPLAY, After(FirstPersonPlayerComponent))
	);

public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

	/*
	*	Callback for before an editable field change happens.
	*/
	void PreEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

	/*
	*	Callback for after an editable field change happens.
	*/
	void PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

	/*
	*	Sends the given event to the given script.
	*/
	void Event(Entity *const RESTRICT entity, const HashString event) NOEXCEPT;

};