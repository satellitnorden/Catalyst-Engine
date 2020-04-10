#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

class FontBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The file.
	const char *RESTRICT _File{ nullptr };

	//The maximum font resolution.
	uint32 _MaximumFontResolution{ 0 };

};