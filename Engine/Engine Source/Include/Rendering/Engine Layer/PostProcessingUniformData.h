#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class PostProcessingUniformData final
{

public:

	//The blur strength.
	float blurStrength{ 0.0f };

	//The chromatic aberration strength.
	float chromaticAberrationStrength{ 0.0f };

	//The saturation strength.
	float saturationStrength{ 1.0f };

	//The sharpen strength.
	float sharpenStrength{ 0.0f };

};