#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class SoundBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The file path.
	const char *RESTRICT _File{ nullptr };

	//The desired sample rate. If the file is not in the same sample rate, the sound will be resampled using linear interpolation.
	float32 _DesiredSampleRate{ 44'100.0f };

};