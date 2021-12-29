#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class EditorPostProcessingSystem final
{

public:

	/*
	*	Updates the editor post-processing system.
	*/
	void Update() NOEXCEPT;

private:

	//Post-processing enabled.
	bool _PostProcessingEnabled{ true };

	//The brightness.
	float32 _Brightness{ 1.0f };

	//The contrast.
	float32 _Contrast{ 1.0f };

	//The chromatic aberration intensity.
	float32 _ChromaticAberrationIntensity{ 0.0025f };

	//The exposure.
	float32 _Exposure{ 1.0f };

	//The saturation.
	float32 _Saturation{ 1.0f };

};

#endif
