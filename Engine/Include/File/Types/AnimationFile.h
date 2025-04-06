#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/AnimationChannel.h>

/*
*	Class representing the raw data read from an animation file (for example .fbx files).
*/
class AnimationFile final
{

public:

	//Enumeration covering all creators.
	enum class Creator : uint8
	{
		BLENDER,

		UNKNOWN
	};

	//The creator.
	Creator _Creator{ Creator::UNKNOWN };

	//The duration, in seconds.
	float32 _Duration;

	//The channels.
	DynamicArray<AnimationChannel> _Channels;

	/*
	*	Post processes this animation file.
	*/
	void PostProcess() NOEXCEPT;

};