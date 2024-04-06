#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>
#include <Math/General/Matrix.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

//Systems.
#include <Systems/RenderingSystem.h>

class RenderingUtilities final
{

public:

	/*
	*	Calculates an axis-aligned bounding box from a set of transformations.
	*/
	FORCE_INLINE static void CalculateAxisAlignedBoundingBoxFromTransformations(const DynamicArray<Matrix4x4> &transformations, const AxisAlignedBoundingBox3D &model_space_bounding_box, AxisAlignedBoundingBox3D *const RESTRICT box) NOEXCEPT
	{
		float extent{ BaseMath::Absolute(model_space_bounding_box._Minimum._X) };
		extent = BaseMath::Maximum<float>(extent, BaseMath::Absolute(model_space_bounding_box._Minimum._Y));
		extent = BaseMath::Maximum<float>(extent, BaseMath::Absolute(model_space_bounding_box._Minimum._Z));
		extent = BaseMath::Maximum<float>(extent, BaseMath::Absolute(model_space_bounding_box._Maximum._X));
		extent = BaseMath::Maximum<float>(extent, BaseMath::Absolute(model_space_bounding_box._Maximum._Y));
		extent = BaseMath::Maximum<float>(extent, BaseMath::Absolute(model_space_bounding_box._Maximum._Z));

		extent *= 2.0f;

		box->_Minimum._X = FLOAT32_MAXIMUM;
		box->_Minimum._Y = FLOAT32_MAXIMUM;
		box->_Minimum._Z = FLOAT32_MAXIMUM;

		box->_Maximum._X = -FLOAT32_MAXIMUM;
		box->_Maximum._Y = -FLOAT32_MAXIMUM;
		box->_Maximum._Z = -FLOAT32_MAXIMUM;

		float largest_scale{ -FLOAT32_MAXIMUM };

		for (const Matrix4x4 &transformation : transformations)
		{
			const Vector3<float> position{ transformation.GetTranslation() };
			const Vector3<float> scale{ transformation.GetScale() };

			box->_Minimum._X = BaseMath::Minimum<float>(box->_Minimum._X, position._X);
			box->_Minimum._Y = BaseMath::Minimum<float>(box->_Minimum._Y, position._Y);
			box->_Minimum._Z = BaseMath::Minimum<float>(box->_Minimum._Z, position._Z);

			box->_Maximum._X = BaseMath::Maximum<float>(box->_Maximum._X, position._X);
			box->_Maximum._Y = BaseMath::Maximum<float>(box->_Maximum._Y, position._Y);
			box->_Maximum._Z = BaseMath::Maximum<float>(box->_Maximum._Z, position._Z);

			largest_scale = BaseMath::Maximum<float>(largest_scale, scale._X);
			largest_scale = BaseMath::Maximum<float>(largest_scale, scale._Y);
			largest_scale = BaseMath::Maximum<float>(largest_scale, scale._Z);
		}

		box->_Minimum._X -= extent * largest_scale;
		box->_Minimum._Y -= extent * largest_scale;
		box->_Minimum._Z -= extent * largest_scale;

		box->_Maximum._X += extent * largest_scale;
		box->_Maximum._Y += extent * largest_scale;
		box->_Maximum._Z += extent * largest_scale;
	}

	/*
	*	Calculates the depth of field size of the given aperture.
	*/
	FORCE_INLINE static NO_DISCARD float32 CalculateDepthOfFieldSize(const float32 aperture) NOEXCEPT
	{
		return BaseMath::LinearlyInterpolate(0.01f, 0.0f, aperture);
	}

	/*
	*	Calculates a directional light matrix from the given parameters.
	*/
	FORCE_INLINE static Matrix4x4 CalculateDirectionalLightMatrix(	const float32 coverage,
																	const float32 depth,
																	const Vector3<float32> &light_direction) NOEXCEPT
	{
		const Matrix4x4 light_matrix{ Matrix4x4::LookAt(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() + -light_direction * depth * 0.5f + RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetForwardVector() * coverage * 0.5f, RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() + light_direction * depth * 0.5f + RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetForwardVector() * coverage * 0.5f, CatalystWorldCoordinateSpace::UP) };
		const Matrix4x4 projection_matrix{ Matrix4x4::Orthographic(-coverage * 0.5f, coverage * 0.5f, -coverage * 0.5f, coverage * 0.5f, 0.0f, depth) };

		return projection_matrix * light_matrix;
	}

	/*
	*	Calculates the ray direction from a screen coordinate.
	*/
	FORCE_INLINE static Vector3<float> CalculateRayDirectionFromScreenCoordinate(const Vector2<float>& screen_coordinate) NOEXCEPT
	{
		const Vector3<float> world_position{ CalculateWorldPositionFromScreenCoordinate(screen_coordinate, 1.0f - FLOAT32_EPSILON) };

		return Vector3<float>::Normalize(world_position - RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition());
	}

	/*
	*	Calculates the screen coordinate of a position.
	*/
	FORCE_INLINE static Vector2<float32> CalculateScreenCoordinate(const Matrix4x4 &view_matrix, const Vector3<float32> &position) NOEXCEPT
	{
		Vector4<float32> view_space_coordinate{ view_matrix * Vector4<float32>(position, 1.0f) };

		const float32 inverse_denominator{ 1.0f / view_space_coordinate._W };

		view_space_coordinate._X *= inverse_denominator;
		view_space_coordinate._Y *= inverse_denominator;

		view_space_coordinate._Y *= -1.0f;

		view_space_coordinate._X = view_space_coordinate._X * 0.5f + 0.5f;
		view_space_coordinate._Y = view_space_coordinate._Y * 0.5f + 0.5f;

		return Vector2<float32>(view_space_coordinate._X, view_space_coordinate._Y);
	}

	/*
	*	Calculates the tangent from the given triangle.
	*/
	FORCE_INLINE static Vector3<float32> CalculateTangent(const StaticArray<Vector3<float32>, 3> &positions, const StaticArray<Vector2<float32>, 3> &texture_coordinates) NOEXCEPT
	{
		const Vector3<float32> edge_1{ positions[1] - positions[0] };
		const Vector3<float32> edge_2{ positions[2] - positions[0] };

		const Vector2<float32> delta_uv_1{ texture_coordinates[1] - texture_coordinates[0] };
		const Vector2<float32> delta_uv_2{ texture_coordinates[2] - texture_coordinates[0] };

		const float32 f{ 1.0f / (delta_uv_1._X * delta_uv_2._Y - delta_uv_2._X * delta_uv_1._Y) };

		return Vector3<float32>
		(
			f * (delta_uv_2._Y * edge_1._X - delta_uv_1._Y * edge_2._X),
			f * (delta_uv_2._Y * edge_1._Y - delta_uv_1._Y * edge_2._Y),
			f * (delta_uv_2._Y * edge_1._Z - delta_uv_1._Y * edge_2._Z)
		);
	}

	/*
	*	Calculates a world position from a screen coordinate.
	*/
	FORCE_INLINE static Vector3<float> CalculateWorldPositionFromScreenCoordinate(const Vector2<float>& screen_coordinate, const float depth) NOEXCEPT
	{
		const Vector2<float> near_plane_coordinate{ screen_coordinate._X * 2.0f - 1.0f, (1.0f - screen_coordinate._Y) * 2.0f - 1.0f };
		Vector4<float> view_space_position{ *RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetInverseProjectionMatrix() * Vector4<float>(Vector3<float>(near_plane_coordinate, depth), 1.0f) };
		const float inverse_view_space_position_denominator{ 1.0f / view_space_position._W };
		view_space_position *= inverse_view_space_position_denominator;
		const Vector4<float> world_space_position{ *RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetInverseCameraMatrix() * view_space_position };

		return Vector3<float>(world_space_position._X, world_space_position._Y, world_space_position._Z);
	}

	/*
	*	Creates a transformations buffer.
	*/
	FORCE_INLINE static void CreateTransformationsBuffer(const DynamicArray<Matrix4x4> &transformations, BufferHandle *const RESTRICT buffer) NOEXCEPT
	{
		const void *RESTRICT data[]{ transformations.Data() };
		const uint64 dataSizes[]{ sizeof(Matrix4x4) * transformations.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, buffer);
		RenderingSystem::Instance->UploadDataToBuffer(data, dataSizes, 1, buffer);
	}

	/*
	*	Generates a mipchain out of the given texture.
	*/
	template <typename TYPE>
	FORCE_INLINE static void GenerateMipChain(const Texture2D<TYPE> &input_texture, const MipmapGenerationMode mipmap_generation_mode, DynamicArray<Texture2D<TYPE>> *const RESTRICT output_textures) NOEXCEPT
	{
		/*
		*	Defines the lowest mip resolution that we generate.
		*	At this resolution, 4096 NxN textures at 4 bytes each equals roughly 1MB,
		*	which is what we can keep loaded at all times.
		*/
		constexpr uint32 LOWEST_MIP_RESOLUTION{ 8 };

		//Copy the original mip level.
		output_textures->Emplace(input_texture);

		//Generate all mip levels.
		uint32 current_width{ input_texture.GetWidth() };
		uint32 current_height{ input_texture.GetHeight() };

		while (current_width > LOWEST_MIP_RESOLUTION && current_height > LOWEST_MIP_RESOLUTION)
		{
			//Cache stuff.
			const uint32 new_width{ current_width >> 1 };
			const uint32 new_height{ current_height >> 1 };

			//Initialize this mip level.
			output_textures->Emplace();
			output_textures->Back().Initialize(new_width, new_height);

			for (uint32 Y{ 0 }; Y < output_textures->Back().GetHeight(); ++Y)
			{
				for (uint32 X{ 0 }; X < output_textures->Back().GetWidth(); ++X)
				{
					//Gather the source texels.
					StaticArray<TYPE, 4> source_texels;

					source_texels[0] = output_textures->At(output_textures->LastIndex() - 1).At(X * 2 + 0, Y * 2 + 0);
					source_texels[1] = output_textures->At(output_textures->LastIndex() - 1).At(X * 2 + 0, Y * 2 + 1);
					source_texels[2] = output_textures->At(output_textures->LastIndex() - 1).At(X * 2 + 1, Y * 2 + 0);
					source_texels[3] = output_textures->At(output_textures->LastIndex() - 1).At(X * 2 + 1, Y * 2 + 1);

					//Blend depending on the mipmap generation mode.
					switch (mipmap_generation_mode)
					{
						case MipmapGenerationMode::NONE:
						{
							ASSERT(false, "Shouldn't have called this function. (:");

							break;
						}

						case MipmapGenerationMode::DEFAULT:
						{
							output_textures->Back().At(X, Y) =	source_texels[0] * 0.25f
																+ source_texels[1] * 0.25f
																+ source_texels[2] * 0.25f
																+ source_texels[3] * 0.25f;

							break;
						}

						case MipmapGenerationMode::MAXIMUM:
						{
							output_textures->Back().At(X, Y) = BaseMath::Maximum<TYPE>(BaseMath::Maximum<TYPE>(source_texels[0], source_texels[1]), BaseMath::Maximum<TYPE>(source_texels[2], source_texels[3]));

							break;
						}

						case MipmapGenerationMode::NORMAL_MAP:
						{
							output_textures->Back().At(X, Y) =	source_texels[0] * 0.25f
																+ source_texels[1] * 0.25f
																+ source_texels[2] * 0.25f
																+ source_texels[3] * 0.25f;

							//TODO: Normalize after blend.

							break;
						}

						case MipmapGenerationMode::OPACITY:
						{
							const TYPE average
							{
								source_texels[0] * 0.25f
								+ source_texels[1] * 0.25f
								+ source_texels[2] * 0.25f
								+ source_texels[3] * 0.25f
							};
							const TYPE maximum
							{
								BaseMath::Maximum<TYPE>
								(
									BaseMath::Maximum<TYPE>(source_texels[0], source_texels[1]),
									BaseMath::Maximum<TYPE>(source_texels[2], source_texels[3])
								)
							};

							output_textures->Back().At(X, Y) = BaseMath::LinearlyInterpolate(average, maximum, 0.5f);

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

			current_width = new_width;
			current_height = new_height;
		}
	}

	/*
	*	Packs a color into an unsigned integer.
	*/
	FORCE_INLINE constexpr static NO_DISCARD uint32 PackColor(const Vector4<float> &color, const bool apply_gamma_correction) NOEXCEPT
	{
		if (apply_gamma_correction)
		{
			float32 first{ 0.0f };
			float32 second{ 0.0f };
			Vector4<float32> corrected;

			first = color._X * color._X;
			second = first * first;
			corrected._X = first * 0.8f + second * 0.2f;

			first = color._Y * color._Y;
			second = first * first;
			corrected._Y = first * 0.8f + second * 0.2f;

			first = color._Z * color._Z;
			second = first * first;
			corrected._Z = first * 0.8f + second * 0.2f;

			first = color._W * color._W;
			second = first * first;
			corrected._W = first * 0.8f + second * 0.2f;

			return	static_cast<uint32>(corrected._X * 255.0f)
					| (static_cast<uint32>(corrected._Y * 255.0f) << 8)
					| (static_cast<uint32>(corrected._Z * 255.0f) << 16)
					| (static_cast<uint32>(corrected._W * 255.0f) << 24);
		}

		else
		{
			return	static_cast<uint32>(color._X * 255.0f)
					| (static_cast<uint32>(color._Y * 255.0f) << 8)
					| (static_cast<uint32>(color._Z * 255.0f) << 16)
					| (static_cast<uint32>(color._W * 255.0f) << 24);
		}
	}

	/*
	*	Transforms an axis aligned bounding box from one space to another.
	*/
	static void TransformAxisAlignedBoundingBox(const AxisAlignedBoundingBox3D &original_box, const Matrix4x4 &transformation, AxisAlignedBoundingBox3D *const RESTRICT new_box) NOEXCEPT;

};