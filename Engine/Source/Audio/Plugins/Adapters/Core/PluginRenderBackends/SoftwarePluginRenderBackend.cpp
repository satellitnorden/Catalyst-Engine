//Header file.
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/SoftwarePluginRenderBackend.h>

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
void SoftwarePluginRenderBackend::Update(const PluginUpdateContext &context) NOEXCEPT
{
	//Calculate the distance from the center of the mouse state.
	const float32 distance_from_center
	{
		BaseMath::Minimum<float32>
		(
			Vector2<float32>::Length
			(
				Vector2<float32>(0.5f, 0.5f) - Vector2<float32>(context._MouseState->_CurrentX, context._MouseState->_CurrentY)
			),
			1.0f
		)
	};

	//Calculate the color.
	const uint32 color
	{
		*Color
		(
			Vector4<float32>
			(
				distance_from_center,
				distance_from_center,
				distance_from_center,
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