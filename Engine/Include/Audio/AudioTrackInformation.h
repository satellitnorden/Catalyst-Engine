#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class AudioTrackInformation final
{

public:

	//The name.
	const char *RESTRICT _Name;

	//The start input channel index. UINT32_MAXIMUM if this track should not receive any inputs.
	uint32 _StartChannelIndex{ UINT32_MAXIMUM };

	//The number of input channels.. UINT32_MAXIMUM if this track should not receive any inputs.
	uint32 _NumberOfInputChannels{ UINT32_MAXIMUM };

};