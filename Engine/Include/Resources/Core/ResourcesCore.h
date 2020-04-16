/*
*	Resources in the Catalyst Engine is dealt with in a few different stages;
*
*	- Building; The process of reading raw resource files (.png, .fbx etc) and packaging it into binary files that's optimized to be loaded at runtime by the Catalyst Engine.
*	- Loading;	The process of reading the binary files into memory and preparing the data for the final processing.
*	- Creation;	The process of taking the data in memory and doing the final processing (for example, sending texture resources data to the GPU).
*/

#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	ANIMATED_MODEL,
	ANIMATION,
	FONT,
	MODEL,
	SOUND,
	TEXTURE_CUBE,
	TEXTURE_2D,
	TEXTURE_3D,

	NUMBER_OF_RESOURCE_TYPES
};

//Enumeration covering all different resource load states.
enum class ResourceLoadState : uint8
{
	NOT_LOADED,
	LOADING,
	LOADED,
	UNLOADING
};