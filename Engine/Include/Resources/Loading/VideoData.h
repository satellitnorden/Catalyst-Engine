#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/VideoResource.h>

class VideoData final
{

public:

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The frames per second.
	float32 _FramesPerSecond;

	//The frames per second reciprocal.
	float32 _FramesPerSecondReciprocal;

	//The frames.
	DynamicArray<VideoResource::Frame> _Frames;

};