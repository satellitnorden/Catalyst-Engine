#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class TextureCubeBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The file path.
	const char *RESTRICT _File{ nullptr };

	//The resolution.
	uint32 _Resolution{ 0 };

};