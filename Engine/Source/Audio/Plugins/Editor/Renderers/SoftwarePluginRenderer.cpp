#if defined(AUDIO_PLUGIN)
//Header file.
#include <Audio/Plugins/Editor/Renderers/SoftwarePluginRenderer.h>

//Math.
#include <Math/Geometry/Triangle2D.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Color.h>

//Systems.
#include <Systems/ContentSystem.h>

namespace Audio
{

	/*
	*	Initializes this plugin renderer.
	*/
	void SoftwarePluginRenderer::Initialize(const Resolution resolution, Audio::PluginWindow *const RESTRICT window) NOEXCEPT
	{
		//Set the window.
		_Window = window;

		//Initialize the buffer.
		_Buffer.Initialize(resolution._Width, resolution._Height);

		//Initialize the last rendered command buffer.
		_LastRenderedCommandBuffer.Initialize(resolution._Width, resolution._Height);

		//Build the texture index to font asset lookup.
		BuildTextureIndexToFontAssetLookup();
	}

	/*
	*	Renders.
	*/
	void SoftwarePluginRenderer::Render(const DynamicArray<UI::RenderCommand> &render_commands) NOEXCEPT
	{
		PROFILING_SCOPE("SoftwarePluginRenderer::Render()");

		//Clear the buffers.
		Memory::Set(_Buffer.Data(), 0, _Buffer.GetWidth() * _Buffer.GetHeight() * sizeof(uint32));
		Memory::Set(_LastRenderedCommandBuffer.Data(), 0, _LastRenderedCommandBuffer.GetWidth() * _LastRenderedCommandBuffer.GetHeight() * sizeof(uint32));

		//Process each render command.
		for (uint64 i{ 0 }; i < render_commands.Size(); ++i)
		{
			//Rasterize the render command.
			Rasterize(i + 1, render_commands[i]);
		}

		//Render to the window!
		_Window->RenderTo(_Buffer.Data());
	}

	/*
	*	Terminates this plugin renderer.
	*/
	void SoftwarePluginRenderer::Terminate() NOEXCEPT
	{

	}

	/*
*	Builds the texture index to font asset lookup.
*/
	void SoftwarePluginRenderer::BuildTextureIndexToFontAssetLookup() NOEXCEPT
	{
		//Retrieve all font assets.
		const HashTable<HashString, Asset *RESTRICT> &assets{ ContentSystem::Instance->GetAllAssetsOfType(FontAsset::TYPE_IDENTIFIER) };

		for (const Asset *const RESTRICT asset : assets.ValueIterator())
		{
			const FontAsset *const RESTRICT font_asset{ static_cast<const FontAsset *const RESTRICT>(asset) };

			_TextureIndexToFontAssetLookup.Add(font_asset->_MasterTextureIndex, font_asset);
		}
	}

	/*
	*	Unpacks a color.
	*/
	NO_DISCARD Vector4<float32> SoftwarePluginRenderer::UnpackColor(const uint32 color) NOEXCEPT
	{
		constexpr float32 MULTIPLIER{ 1.0f / static_cast<float32>(UINT8_MAXIMUM) };

		Vector4<float32> output{ 0.0f, 0.0f, 0.0f, 1.0f };

		output._B = static_cast<float32>(color & UINT8_MAXIMUM) * MULTIPLIER;
		output._G = static_cast<float32>((color >> 8) & UINT8_MAXIMUM) * MULTIPLIER;
		output._R = static_cast<float32>((color >> 16) & UINT8_MAXIMUM) * MULTIPLIER;
		output._A = static_cast<float32>((color >> 24) & UINT8_MAXIMUM) * MULTIPLIER;

		return output;
	}

	/*
	*	Packs a color.
	*/
	NO_DISCARD uint32 SoftwarePluginRenderer::PackColor(const Vector4<float32> &color) NOEXCEPT
	{
		uint32 output{ 0 };

		output |= static_cast<uint32>(color._B * static_cast<float32>(UINT8_MAXIMUM));
		output |= static_cast<uint32>(color._G * static_cast<float32>(UINT8_MAXIMUM)) << 8;
		output |= static_cast<uint32>(color._R * static_cast<float32>(UINT8_MAXIMUM)) << 16;
		output |= static_cast<uint32>(color._A * static_cast<float32>(UINT8_MAXIMUM)) << 24;

		return output;
	}

	/*
	*	Rasterizes the given render command.
	*/
	void SoftwarePluginRenderer::Rasterize(const uint32 render_command_index, const UI::RenderCommand &render_command) NOEXCEPT
	{
		PROFILING_SCOPE("SoftwarePluginRenderer::Rasterize()");

		/*
		*	Triangle raster class definition.
		*/
		class TriangleRaster final
		{

		public:

			//The triangle.
			Triangle2D _Triangle;

			//The texture coordinates.
			StaticArray<Vector2<float32>, 3> _TextureCoordinates;

		};

		//Split the render command into triangles.
		StaticArray<TriangleRaster, 2> triangles;

		triangles[0]._Triangle._Vertices[0]._X = (render_command._Positions[0]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
		triangles[0]._Triangle._Vertices[0]._Y = (1.0f - (render_command._Positions[0]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());

		triangles[0]._TextureCoordinates[0] = render_command._TextureCoordinates[0];

		triangles[0]._Triangle._Vertices[1]._X = (render_command._Positions[1]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
		triangles[0]._Triangle._Vertices[1]._Y = (1.0f - (render_command._Positions[1]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());

		triangles[0]._TextureCoordinates[1] = render_command._TextureCoordinates[1];

		triangles[0]._Triangle._Vertices[2]._X = (render_command._Positions[2]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
		triangles[0]._Triangle._Vertices[2]._Y = (1.0f - (render_command._Positions[2]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());

		triangles[0]._TextureCoordinates[2] = render_command._TextureCoordinates[2];

		triangles[1]._Triangle._Vertices[0]._X = (render_command._Positions[0]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
		triangles[1]._Triangle._Vertices[0]._Y = (1.0f - (render_command._Positions[0]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());

		triangles[1]._TextureCoordinates[0] = render_command._TextureCoordinates[0];

		triangles[1]._Triangle._Vertices[1]._X = (render_command._Positions[2]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
		triangles[1]._Triangle._Vertices[1]._Y = (1.0f - (render_command._Positions[2]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());

		triangles[1]._TextureCoordinates[1] = render_command._TextureCoordinates[2];

		triangles[1]._Triangle._Vertices[2]._X = (render_command._Positions[3]._X * 0.5f + 0.5f) * static_cast<float32>(_Buffer.GetWidth());
		triangles[1]._Triangle._Vertices[2]._Y = (1.0f - (render_command._Positions[3]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Buffer.GetHeight());

		triangles[1]._TextureCoordinates[2] = render_command._TextureCoordinates[3];

		//Calculate the axis aligned bounding box.
		const AxisAlignedBoundingBox2D bounding_box
		{
			Vector2<float32>(triangles[0]._Triangle._Vertices[0]._X, triangles[0]._Triangle._Vertices[0]._Y),
			Vector2<float32>(triangles[0]._Triangle._Vertices[2]._X, triangles[0]._Triangle._Vertices[2]._Y)
		};

		//Rasterize each triangle.
		for (const TriangleRaster &triangle : triangles)
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

			for (const Vector2<float32> &vertex : triangle._Triangle._Vertices)
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
					//If this command has already rendered on this pixel, skip it.
					if (_LastRenderedCommandBuffer.At(X, Y) == render_command_index)
					{
						continue;
					}

					//Calculate the point.
					const Vector2<float32> point{ static_cast<float32>(X) + 0.5f, static_cast<float32>(Y) + 0.5f };

					//Discard if the point is not inside.
					if (!Triangle2D::IsInside(triangle._Triangle, point))
					{
						continue;
					}

					//Calculate the barycentric coordinates.
					const Vector3<float32> barycentric_coordinates{ Triangle2D::CalculateBarycentricCoordinates(triangle._Triangle, point) };

					//This point is inside the triangle - Time to call the fragment function!
					const Vector2<float32> pixel_position{ point._X, point._Y };
					const Vector2<float32> texture_coordinate
					{
						triangle._TextureCoordinates[0] * barycentric_coordinates[0]
						+ triangle._TextureCoordinates[1] * barycentric_coordinates[1]
						+ triangle._TextureCoordinates[2] * barycentric_coordinates[2]
					};

					//Run the fragment shader!
					const Vector4<float32> fragment{ Fragment(render_command, bounding_box, pixel_position, texture_coordinate) };

					//Retrieve the current fragment.
					const Vector4<float32> current{ UnpackColor(_Buffer.At(X, Y)) };

					//Perform the blend.
					const Vector4<float32> blend
					{
						BaseMath::LinearlyInterpolate(current._R, fragment._R, fragment._A),
						BaseMath::LinearlyInterpolate(current._G, fragment._G, fragment._A),
						BaseMath::LinearlyInterpolate(current._B, fragment._B, fragment._A),
						1.0f
					};

					//Write into the buffer!
					_Buffer.At(X, Y) = PackColor(blend);

					//Update the last rendered command buffer.
					_LastRenderedCommandBuffer.At(X, Y) = render_command_index;
				}
			}
		}
	}

	/*
	*	The fragment function.
	*/
	NO_DISCARD Vector4<float32> SoftwarePluginRenderer::Fragment
	(
		const UI::RenderCommand &render_command,
		const AxisAlignedBoundingBox2D &bounding_box,
		const Vector2<float32> pixel_position,
		const Vector2<float32> texture_coordinate
	) NOEXCEPT
	{
		PROFILING_SCOPE("SoftwarePluginRenderer::Fragment()");

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
				color = Vector4<float32>(1.0f, 0.0f, 0.0f, 1.0f);

				break;
			}

			case UI::RenderCommand::Mode::TEXT:
			{
				//Retrieve the font asset.
				const FontAsset *const RESTRICT font_asset{ _TextureIndexToFontAssetLookup[render_command._ColorOrTexture] };

				//Sample the font signed distance.
				const float32 text_signed_distance{ SampleConvert(font_asset->_Texture2D, texture_coordinate) };

				//Calculate the opacity.
				const float32 opacity{ BaseMath::SmoothStep(text_signed_distance, 0.5f - render_command._ModeParameter1_float32, 0.5f) };

				//Write the color!
				color = Vector4<float32>(1.0f, 1.0f, 1.0f, opacity);

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

}
#endif