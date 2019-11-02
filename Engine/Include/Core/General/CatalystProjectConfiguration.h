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

	/*
	*	The project name.
	*	Will be the name of the window.
	*	Recommended: "Catalyst Engine Project".
	*/
	DynamicString _ProjectName;

	/*
	*	The initialization function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
	void(*_InitializationFunction)();

	/*
	*	The pre update function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
	void(*_PreUpdateFunction)(const UpdateContext *const RESTRICT);

	/*
	*	The logic update function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
	void(*_LogicUpdateFunction)(const UpdateContext *const RESTRICT);

	/*
	*	The physics update function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
	void(*_PhysicsUpdateFunction)(const UpdateContext *const RESTRICT);

	/*
	*	The physics update function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
	void(*_RenderUpdateFunction)(const UpdateContext *const RESTRICT);

	/*
	*	The post update function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
	void(*_PostUpdateFunction)(const UpdateContext *const RESTRICT);

	/*
	*	The post update function.
	*	Recommended: A non-nullptr function pointer to a game system function.
	*/
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

	/*
	*	The resolution.
	*	Recommended: 1'920, 1'080.
	*/
	Resolution _Resolution;

	/*
	*	The resolution scale.
	*	Recommended: 1.0f.
	*/
	float _ResolutionScale;

	/*
	*	The view distance.
	*	Recommended: 1'024.0f.
	*/
	float _ViewDistance;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectSoundConfiguration final
{

public:

	/*
	*	The maximum number of channels.
	*	Recommended: 64;
	*/
	uint32 _MaximumNumberOfChannels;

	/*
	*	The number of mixing buffers.
	*	Recommended: 4.
	*/
	uint8 _NumberOfMixingBuffers;

	/*
	*	The mixing buffer size.
	*	Recommended: 1'024.
	*/
	uint32 _MixingBufferSize;

	/*
	*	Default constructor.
	*/
	CatalystProjectSoundConfiguration() NOEXCEPT
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

	//The project sound configuration.
	CatalystProjectSoundConfiguration _SoundConfiguration;

};