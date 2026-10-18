//Header file.
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/SoftwarePluginRenderBackend.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Color.h>

/*
*	Default constructor.
*/
SoftwarePluginRenderBackend::SoftwarePluginRenderBackend(const Audio::Plugin *const RESTRICT plugin) NOEXCEPT
	:
	PluginRenderBackend(plugin)
{
	//Retrieve the resolution.
	const Resolution resolution{ plugin->GetResolution() };

	//Initialize the data.
	_Data.Initialize(resolution._Width, resolution._Height);
}

/*
*	Updates this render backend.
*/
void SoftwarePluginRenderBackend::Update() NOEXCEPT
{
	//Just draw a random color. (:
	const float32 random_number{ CatalystRandomMath::RandomFloat() };

	const uint32 color
	{
		*Color
		(
			Vector4<float32>
			(
				random_number,
				random_number,
				random_number,
				1.0f
			)
		).Data()
	};

	for (uint32 Y{ 0 }; Y < _Data.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < _Data.GetWidth(); ++X)
		{
			_Data.At(X, Y) = color;
		}
	}
}