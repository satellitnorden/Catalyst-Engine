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

DECLARE_COMPONENT
(
	ScriptComponent,
	ScriptInitializationData,
	ScriptInstanceData,
public:
	
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

);