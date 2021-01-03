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

class CatalystProjectInputConfiguration final
{

public:

	/*
	*	The number of supported gamepads.
	*	Recommended: 1 for singeleplayer games, 1-4 for multiplayer games.
	*/
	uint8 _NumberOfSupportedGamepads;

	/*
	*	Default constructor.
	*/
	CatalystProjectInputConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectRenderingConfiguration final
{

public:

	/*
	*	Denotes the focused frame time, denoted in seconds. Set to <= 0.0f if it doesn't matter.
	*	Recommended: 0.016f
	*/
	float32 _FocusedFrameTime;

	/*
	*	Denotes the unfocused frame time, denoted in seconds. Set to <= 0.0f if it doesn't matter.
	*	Recommended: 0.1f
	*/
	float32 _UnfocusedFrameTime;

	/*
	*	Denotes if the game should start in fullscreen initially.
	*	Recommended: true/false.
	*/
	bool _InitialFullScreen;

	/*
	*	The resolution.
	*	Recommended: 1'920, 1'080.
	*/
	Resolution _Resolution;

	/*
	*	The resolution scale.
	*	Recommended: 1.0f.
	*/
	float32 _ResolutionScale;

	/*
	*	The view distance.
	*	Recommended: 1'024.0f.
	*/
	float32 _ViewDistance;

	/*
	*	The shadow map resolution.
	*	Recommended: 1'024.
	*/
	uint32 _ShadowMapResolution;

	/*
	*	Default constructor.
	*/
	CatalystProjectRenderingConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectTerrainConfiguration final
{

public:

	/*
	*	The patch size.
	*	Defines the size of each root node patch.
	*	Recommended: 1'024.0f.
	*/
	float32 _PatchSize;

	/*
	*	The patch resolution.
	*	Defines the resolution of the terrain plane.
	*	Higher resolutions means better quality for displacement, but worse performance.
	*	MUST be a power of two plus one.
	*	Recommended: 65.
	*/
	uint32 _PatchResolution;

	/*
	*	The maximum quad tree depth.
	*	Defines the maximum number of times each quad tree node can be subdivided.
	*	Higher depths means better quality for displacement, but worse performance.
	*	Recommended: 8.
	*/
	uint8 _MaximumQuadTreeDepth;

	/*
	*	Default constructor.
	*/
	CatalystProjectTerrainConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectWorldConfiguration final
{

public:

	/*
	*	The world grid size.
	*	The world is divided into a three-dimensional grid in the Catalyst Engine™ to enable infinite worlds. 
	*	This is the size of each grid cell.
	*	Recommended: 1'024.0f.
	*/
	float32 _WorldGridSize;

	/*
	*	Default constructor.
	*/
	CatalystProjectWorldConfiguration() NOEXCEPT
	{

	}

};

class CatalystProjectConfiguration final
{

public:

	//The project general configuration.
	CatalystProjectGeneralConfiguration _GeneralConfiguration;

	//The project input configuration.
	CatalystProjectInputConfiguration _InputConfiguration;

	//The project rendering configuration.
	CatalystProjectRenderingConfiguration _RenderingConfiguration;

	//The project terrain configuration.
	CatalystProjectTerrainConfiguration _TerrainConfiguration;

	//The project world configuration.
	CatalystProjectWorldConfiguration _WorldConfiguration;

};