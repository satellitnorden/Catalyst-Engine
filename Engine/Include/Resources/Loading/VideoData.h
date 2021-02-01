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

	//The frames.
	DynamicArray<VideoResource::Frame> _Frames;

};