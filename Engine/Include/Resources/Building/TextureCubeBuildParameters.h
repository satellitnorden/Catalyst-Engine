#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class TextureCubeBuildParameters final
{

public:

	//Type aliases.
	using ProceduralFunction = Vector4<float32>(*)(const Vector3<float32> &direction, void *const RESTRICT user_data);

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource ID.
	const char *RESTRICT _ID{ nullptr };

	//The file path.
	const char *RESTRICT _File{ nullptr };

	//The default resolution.
	uint32 _DefaultResolution{ 0 };

	//The procedural function.
	ProceduralFunction _ProceduralFunction{ nullptr };

	//The procedural function user data.
	void *RESTRICT _ProceduralFunctionUserData{ nullptr };

	//Denotes whether or not to supersample the procedural function.
	bool _ProceduralFunctionSuperSample{ false };

};