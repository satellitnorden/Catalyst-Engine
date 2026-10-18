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

	//Initialize the buffer.
	_Buffer.Initialize(resolution._Width, resolution._Height);
}

/*
*	Updates this render backend.
*/
void SoftwarePluginRenderBackend::Update(const PluginUpdateContext &context, const DynamicArray<UI::RenderCommand> &render_commands) NOEXCEPT
{
	//Clear the buffer.
	Memory::Set(_Buffer.Data(), 0, _Buffer.GetWidth() * _Buffer.GetHeight() * sizeof(uint32));

	//Process each render command.
	for (const UI::RenderCommand &render_command : render_commands)
	{
		switch (render_command._Mode)
		{
			case UI::RenderCommand::Mode::COLOR:
			{
				//Treat the positions as a simple bounding box for now, and calculate the minimum/maximum coordinates.
				const Vector2<uint32> minimum
				{
					BaseMath::Round<uint32>((render_command._Positions[0]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth())),
					BaseMath::Round<uint32>((1.0f - (render_command._Positions[0]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight()))
				};
				const Vector2<uint32> maximum
				{
					BaseMath::Round<uint32>((render_command._Positions[2]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth())),
					BaseMath::Round<uint32>((1.0f - (render_command._Positions[2]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight()))
				};

				//Draw for each pixel.
				for (uint32 Y{ minimum._Y }; Y <= maximum._Y; ++Y)
				{
					for (uint32 X{ minimum._X }; X <= maximum._X; ++X)
					{
						//Retrieve the current value.
						const Vector4<float32> current{ Color(_Buffer.At(X, Y)).Get() };

						//Retrieve the color.
						const Vector4<float32> color{ Color(render_command._ColorOrTexture).Get() };

						//Retrieve the color/opacity.
						const Vector4<float32> color_opacity{ Color(render_command._ColorOpacity).Get() };

						//Calculate the destination.
						const Vector4<float32> destination{ color * color_opacity };

						//Perform the blend.
						const Vector4<float32> blend
						{
							BaseMath::LinearlyInterpolate(current._R, destination._R, destination._A),
							BaseMath::LinearlyInterpolate(current._G, destination._G, destination._A),
							BaseMath::LinearlyInterpolate(current._B, destination._B, destination._A),
							1.0f
						};

						//Write into the buffer!
						_Buffer.At(X, Y) = *Color(blend).Data();
					}
				}

				break;
			}

			case UI::RenderCommand::Mode::TEXTURE:
			{
				//TODO: Support this. (:

				break;
			}

			case UI::RenderCommand::Mode::TEXT:
			{
				//TODO: Support this. (:

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
}