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
	constexpr HashString ANIMATED_MODEL_TYPE_IDENTIFIER{ "AnimatedModel" };
	constexpr HashString ANIMATION_TYPE_IDENTIFIER{ "Animation" };
	constexpr HashString FONT_TYPE_IDENTIFIER{ "Font" };
	constexpr HashString MODEL_TYPE_IDENTIFIER{ "Model" };
	constexpr HashString SHADER_TYPE_IDENTIFIER{ "Shader" };
	constexpr HashString SOUND_TYPE_IDENTIFIER{ "Sound" };
	constexpr HashString TEXTURE_CUBE_TYPE_IDENTIFIER{ "TextureCube" };
	constexpr HashString TEXTURE_2D_TYPE_IDENTIFIER{ "Texture2D" };
	constexpr HashString TEXTURE_3D_TYPE_IDENTIFIER{ "Texture3D" };
}