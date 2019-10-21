/*
*	Resources in the Catalyst Engine is dealt with in a lot of different stages;
*
*	- Reading;	The process of reading raw resource files (.png, .fbx etc) and transforming them into the format that the Catalyst Engine is comfortable with.
*	- Building; The process of taking the raw data that was just read and packaging it into binary files that's optimized to be loaded at runtime by the Catalyst Engine.
*	- Loading;	The process of reading the binary files into memory and preparing the data for the final processing.
*	- Creation;	The process of taking the data in memory and doing the final processing (for example, sending texture resources data to the GPU).
*/

#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	AnimatedModel,
	Animation,
	Font,
	Model,
	SoundBank,
	TextureCube,
	Texture2D,

	NumberOfResourceTypes
};