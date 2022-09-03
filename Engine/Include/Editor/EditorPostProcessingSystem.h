#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

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

	//The tint color.
	Vector3<float32> _TintColor{ 1.0f, 1.0f, 1.0f };

	//The tint intensity.
	float32 _TintIntensity{ 0.0f };

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
