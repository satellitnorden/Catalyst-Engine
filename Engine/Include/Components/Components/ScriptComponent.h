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

DECLARE_COMPONENT(ScriptComponent, ScriptInitializationData, ScriptInstanceData);