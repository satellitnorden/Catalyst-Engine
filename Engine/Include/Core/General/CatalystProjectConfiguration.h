#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>
#include <Core/General/DynamicString.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

class CatalystProjectGeneralConfiguration final
{

public:

	//The project name.
	DynamicString _ProjectName;

	//The initialization function.
	void(*_InitializationFunction)();

	//The pre update function.
	void(*_PreUpdateFunction)(const UpdateContext *const RESTRICT);

	//The logic update function.
	void(*_LogicUpdateFunction)(const UpdateContext *const RESTRICT);

	//The physics update function.
	void(*_PhysicsUpdateFunction)(const UpdateContext *const RESTRICT);

	//The render update function.
	void(*_RenderUpdateFunction)(const UpdateContext *const RESTRICT);

	//The post update function.
	void(*_PostUpdateFunction)(const UpdateContext *const RESTRICT);

	//The termination function.
	void(*_TerminationFunction)();

	/*
	*	Default constructor.
	*/
	CatalystProjectGeneralConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectRenderingConfiguration final
{

public:

	//The resolution.
	Resolution _Resolution;

	//The resolution scale.
	float _ResolutionScale;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectConfiguration final
{

public:

	//The project general configuration.
	CatalystProjectGeneralConfiguration _GeneralConfiguration;

	//The project rendering configuration.
	CatalystProjectRenderingConfiguration _RenderingConfiguration;

};