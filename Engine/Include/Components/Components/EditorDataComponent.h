#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Math.
#include <Math/General/EulerAngles.h>

class EditorDataInitializationData final : public ComponentInitializationData
{

public:

	/*
	*	The rotation.
	*	Stored as Euler Angles here, since that is more editor-friendly,
	*	and leads to less instabilities if we avoid converting from/to quaternions all the time.
	*/
	EulerAngles _Rotation;

};

class EditorDataInstanceData final
{

public:

	/*
	*	The rotation.
	*	Stored as Euler Angles here, since that is more editor-friendly,
	*	and leads to less instabilities if we avoid converting from/to quaternions all the time.
	*/
	EulerAngles _Rotation;

};

DECLARE_COMPONENT(EditorDataComponent, EditorDataInitializationData, EditorDataInstanceData);