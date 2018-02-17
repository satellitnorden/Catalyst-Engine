#pragma once

//Engine core.
#include <EngineCore.h>

class PostProcessingUniformData final
{

public:

	//The blur amount.
	float blurAmount{ 0.0f };

	//The chromatic aberration amount.
	float chromaticAberrationAmount{ 0.0f };

	//The saturation.
	float saturation{ 1.0f };

	//The sharpen amount.
	float sharpenAmount{ 0.0f };

};