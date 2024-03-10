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
#include <Core/General/HashString.h>

//Enumeration covering all different resource load states.
enum class ResourceLoadState : uint8
{
	NOT_LOADED,
	LOADING,
	LOADED,
	UNLOADING
};

//Resource constants.
namespace ResourceConstants
{
	//The type identifiers.
	const extern HashString ANIMATED_MODEL_TYPE_IDENTIFIER;
	const extern HashString ANIMATION_TYPE_IDENTIFIER;
	const extern HashString FONT_TYPE_IDENTIFIER;
	const extern HashString LEVEL_TYPE_IDENTIFIER;
	const extern HashString RAW_DATA_TYPE_IDENTIFIER;
	const extern HashString RENDER_PIPELINE_TYPE_IDENTIFIER;
	const extern HashString SHADER_TYPE_IDENTIFIER;
	const extern HashString SOUND_TYPE_IDENTIFIER;
	const extern HashString TEXTURE_CUBE_TYPE_IDENTIFIER;
	const extern HashString TEXTURE_3D_TYPE_IDENTIFIER;
	const extern HashString VIDEO_TYPE_IDENTIFIER;
}