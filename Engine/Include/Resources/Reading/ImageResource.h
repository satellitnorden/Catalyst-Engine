#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ImageResource final
{


public:

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The number of channels.
	uint8 NumberOfChannels;

	//The data.
	byte *RESTRICT _Data;

};