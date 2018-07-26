#pragma once

//Core.
#include <Core/EngineCore.h>

class PostProcessingUniformData final
{

public:

	//The bloom strength.
	float bloomStrength;

	//The blur strength.
	float blurStrength;

	//The chromatic aberration strength.
	float chromaticAberrationStrength;

	//The saturation strength.
	float saturationStrength;

	//The sharpen strength.
	float sharpenStrength;

};