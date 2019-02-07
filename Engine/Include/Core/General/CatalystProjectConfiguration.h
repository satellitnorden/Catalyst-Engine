#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>
#include <Core/General/DynamicString.h>

//Rendering.
#include <Rendering/Engine/Resolution.h>

class CatalystProjectGeneralConfiguration final
{

public:

	//The project name.
	DynamicString _ProjectName;

	//The initialization function.
	void(*_InitializationFunction)();

	//The pre update function.
	void(*_PreUpdateFunction)(const UpdateContext *const RESTRICT);

	//The update function.
	void(*_UpdateFunction)(const UpdateContext *const RESTRICT);

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

	//The shadow map resolution.
	uint32 _ShadowMapResolution;

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