//Header file.
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/SoftwarePluginRenderBackend.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

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
		//Rasterize the render command.
		Rasterize(render_command);
	}
}

/*
*	Rasterizes the given render command.
*/
void SoftwarePluginRenderBackend::Rasterize(const UI::RenderCommand &render_command) NOEXCEPT
{
	//Split the render command into triangles.
	StaticArray<Triangle, 2> triangles;

	triangles[0]._Vertices[0]._X = (render_command._Positions[0]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
	triangles[0]._Vertices[0]._Y = (1.0f - (render_command._Positions[0]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());
	triangles[0]._Vertices[0]._Z = 0.0f;

	triangles[0]._Vertices[1]._X = (render_command._Positions[1]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
	triangles[0]._Vertices[1]._Y = (1.0f - (render_command._Positions[1]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());
	triangles[0]._Vertices[1]._Z = 0.0f;

	triangles[0]._Vertices[2]._X = (render_command._Positions[2]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
	triangles[0]._Vertices[2]._Y = (1.0f - (render_command._Positions[2]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());
	triangles[0]._Vertices[2]._Z = 0.0f;

	triangles[1]._Vertices[0]._X = (render_command._Positions[0]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
	triangles[1]._Vertices[0]._Y = (1.0f - (render_command._Positions[0]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());
	triangles[1]._Vertices[0]._Z = 0.0f;

	triangles[1]._Vertices[1]._X = (render_command._Positions[2]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
	triangles[1]._Vertices[1]._Y = (1.0f - (render_command._Positions[2]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());
	triangles[1]._Vertices[1]._Z = 0.0f;

	triangles[1]._Vertices[2]._X = (render_command._Positions[3]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
	triangles[1]._Vertices[2]._Y = (1.0f - (render_command._Positions[3]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());
	triangles[1]._Vertices[2]._Z = 0.0f;

	//Calculate the axis aligned bounding box.
	const AxisAlignedBoundingBox2D bounding_box
	{
		Vector2<float32>(triangles[0]._Vertices[0]._X, triangles[0]._Vertices[0]._Y),
		Vector2<float32>(triangles[0]._Vertices[2]._X, triangles[0]._Vertices[2]._Y)
	};

	//Rasterize each triangle.
	for (const Triangle &triangle : triangles)
	{
		//Calculate the minimum/maximum.
		Vector2<uint32> minimum
		{
			_Buffer.GetWidth() - 1,
			_Buffer.GetHeight() - 1
		};

		Vector2<uint32> maximum
		{
			0,
			0
		};

		for (const Vector3<float32> &vertex : triangle._Vertices)
		{
			minimum._X = BaseMath::Minimum<uint32>(minimum._X, BaseMath::Floor<uint32>(vertex._X));
			minimum._Y = BaseMath::Minimum<uint32>(minimum._Y, BaseMath::Floor<uint32>(vertex._Y));

			maximum._X = BaseMath::Maximum<uint32>(maximum._X, BaseMath::Ceiling<uint32>(vertex._X));
			maximum._Y = BaseMath::Maximum<uint32>(maximum._Y, BaseMath::Ceiling<uint32>(vertex._Y));
		}

		//Iterate over the pixels.
		for (uint32 Y{ minimum._Y }; Y <= maximum._Y; ++Y)
		{
			for (uint32 X{ minimum._X }; X <= maximum._X; ++X)
			{
				//Calculate the point.
				const Vector3<float32> point{ static_cast<float32>(X) + 0.5f, static_cast<float32>(Y) + 0.5f, 0.0f };

				//Calculate the barycentric coordinates.
				const Vector3<float32> barycentric_coordinates{ CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, point) };

				//If any of the barycentric coordinates are negative, then the point is outside.
				if (barycentric_coordinates._X < 0.0f || barycentric_coordinates._Y < 0.0f || barycentric_coordinates._Z < 0.0f)
				{
					continue;
				}

				//This point is inside the triangle - Time to call the fragment function!
				const Vector2<float32> pixel_position{ point._X, point._Y };
				const Vector2<float32> texture_coordinate{ 0.0f, 0.0f }; //TODO!

				//Run the fragment shader!
				const Vector4<float32> fragment{ Fragment(render_command, bounding_box, pixel_position, texture_coordinate) };

				//Retrieve the current fragment.
				const Vector4<float32> current{ Color(_Buffer.At(X, Y)).Get() };

				//Perform the blend.
				const Vector4<float32> blend
				{
					BaseMath::LinearlyInterpolate(current._R, fragment._R, fragment._A),
					BaseMath::LinearlyInterpolate(current._G, fragment._G, fragment._A),
					BaseMath::LinearlyInterpolate(current._B, fragment._B, fragment._A),
					1.0f
				};

				//Write into the buffer!
				_Buffer.At(X, Y) = *Color(blend).Data();
			}
		}
	}
}

/*
*	The fragment function.
*/
NO_DISCARD Vector4<float32> SoftwarePluginRenderBackend::Fragment
(
	const UI::RenderCommand &render_command,
	const AxisAlignedBoundingBox2D &bounding_box,
	const Vector2<float32> pixel_position,
	const Vector2<float32> texture_coordinate
) NOEXCEPT
{
	//Calculate the color.
	Vector4<float32> color{ 0.0f, 0.0f, 0.0f, 0.0f };

	switch (render_command._Mode)
	{
		case UI::RenderCommand::Mode::NONE:
		{
			ASSERT(false, "Render command needs a mode!");

			break;
		}

		case UI::RenderCommand::Mode::COLOR:
		{
			color = Color(render_command._ColorOrTexture).Get();

			break;
		}

		case UI::RenderCommand::Mode::TEXTURE:
		{
			//TODO!

			break;
		}

		case UI::RenderCommand::Mode::TEXT:
		{
			//TODO!

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Apply the color/opacity.
	color *= Color(render_command._ColorOpacity).Get();

	//Apply the shape.
	switch (render_command._Shape)
	{
		case UI::RenderCommand::Shape::SOLID_CIRCLE:
		{
			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(bounding_box) };

			//Calculate the distance to the center.
			const float32 distance_to_center{ Vector2<float32>::Length(center - pixel_position) };

			//Calculate the signed distances.
			const float32 signed_distance{ distance_to_center - render_command._ShapeParameter1_float32 };

			//Calculate the opacity.
			const float32 opacity{ 1.0f - BaseMath::SmoothStep(signed_distance, 0.0f, 1.0f) };

			//Apply the opacity.
			color._A *= opacity;

			break;
		}

		case UI::RenderCommand::Shape::HOLLOW_CIRCLE:
		{
			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(bounding_box) };

			//Calculate the distance to the center.
			const float32 distance_to_center{ Vector2<float32>::Length(center - pixel_position) };

			//Calculate the signed distances.
			const float32 signed_distance_1{ distance_to_center - render_command._ShapeParameter1_float32 };
			const float32 signed_distance_2{ (distance_to_center - render_command._ShapeParameter2_float32) * -1.0f };

			const float32 signed_distance{ BaseMath::Maximum<float32>(signed_distance_1, signed_distance_2) };

			//Calculate the opacity.
			const float32 opacity{ 1.0f - BaseMath::SmoothStep(signed_distance, 0.0f, 1.0f) };

			//Apply the opacity.
			color._A *= opacity;

			break;
		}

		//Some shapes ignored for now...
	}

	//Return the color.
	return color;
}