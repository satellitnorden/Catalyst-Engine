#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Perceiver.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Matrix.h>

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
	FORCE_INLINE static void CalculateAxisAlignedBoundingBoxFromTransformations(const DynamicArray<Matrix4x4> &transformations, const AxisAlignedBoundingBox3 &model_space_bounding_box, AxisAlignedBoundingBox3 *const RESTRICT box) NOEXCEPT
	{
		float extent{ CatalystBaseMath::Absolute(model_space_bounding_box._Minimum._X) };
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(model_space_bounding_box._Minimum._Y));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(model_space_bounding_box._Minimum._Z));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(model_space_bounding_box._Maximum._X));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(model_space_bounding_box._Maximum._Y));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(model_space_bounding_box._Maximum._Z));

		extent *= 2.0f;

		box->_Minimum._X = FLOAT_MAXIMUM;
		box->_Minimum._Y = FLOAT_MAXIMUM;
		box->_Minimum._Z = FLOAT_MAXIMUM;

		box->_Maximum._X = -FLOAT_MAXIMUM;
		box->_Maximum._Y = -FLOAT_MAXIMUM;
		box->_Maximum._Z = -FLOAT_MAXIMUM;

		float largest_scale{ -FLOAT_MAXIMUM };

		for (const Matrix4x4 &transformation : transformations)
		{
			const Vector3<float> position{ transformation.GetTranslation() };
			const Vector3<float> scale{ transformation.GetScale() };

			box->_Minimum._X = CatalystBaseMath::Minimum<float>(box->_Minimum._X, position._X);
			box->_Minimum._Y = CatalystBaseMath::Minimum<float>(box->_Minimum._Y, position._Y);
			box->_Minimum._Z = CatalystBaseMath::Minimum<float>(box->_Minimum._Z, position._Z);

			box->_Maximum._X = CatalystBaseMath::Maximum<float>(box->_Maximum._X, position._X);
			box->_Maximum._Y = CatalystBaseMath::Maximum<float>(box->_Maximum._Y, position._Y);
			box->_Maximum._Z = CatalystBaseMath::Maximum<float>(box->_Maximum._Z, position._Z);

			largest_scale = CatalystBaseMath::Maximum<float>(largest_scale, scale._X);
			largest_scale = CatalystBaseMath::Maximum<float>(largest_scale, scale._Y);
			largest_scale = CatalystBaseMath::Maximum<float>(largest_scale, scale._Z);
		}

		box->_Minimum._X -= extent * largest_scale;
		box->_Minimum._Y -= extent * largest_scale;
		box->_Minimum._Z -= extent * largest_scale;

		box->_Maximum._X += extent * largest_scale;
		box->_Maximum._Y += extent * largest_scale;
		box->_Maximum._Z += extent * largest_scale;
	}

	/*
	*	Calculates the ray direction from a screen coordinate.
	*/
	FORCE_INLINE static Vector3<float> CalculateRayDirectionFromScreenCoordinate(const Vector2<float>& screen_coordinate) NOEXCEPT
	{
		const Vector3<float> world_position{ CalculateWorldPositionFromScreenCoordinate(screen_coordinate, 1.0f - std::numeric_limits<float>::epsilon()) };

		return Vector3<float>::Normalize(world_position - Perceiver::Instance->GetWorldTransform().GetLocalPosition());
	}

	/*
	*	Calculates the screen coordinate of a position.
	*/
	FORCE_INLINE static Vector2<float> CalculateScreenCoordinate(const Matrix4x4 &view_matrix, const Vector3<float>& position) NOEXCEPT
	{
		Vector4<float> view_space_coordinate{ view_matrix * Vector4<float>(position, 1.0f) };

		const float inverse_denominator{ 1.0f / view_space_coordinate._W };

		view_space_coordinate._X *= inverse_denominator;
		view_space_coordinate._Y *= inverse_denominator;

		view_space_coordinate._X = view_space_coordinate._X * 0.5f + 0.5f;
		view_space_coordinate._Y = view_space_coordinate._Y * 0.5f + 0.5f;

		return Vector2<float>(view_space_coordinate._X, view_space_coordinate._Y);
	}

	/*
	*	Calculates a world position from a screen coordinate.
	*/
	FORCE_INLINE static Vector3<float> CalculateWorldPositionFromScreenCoordinate(const Vector2<float>& screen_coordinate, const float depth) NOEXCEPT
	{
		const Vector2<float> near_plane_coordinate{ screen_coordinate._X * 2.0f - 1.0f, (1.0f - screen_coordinate._Y) * 2.0f - 1.0f };
		Vector4<float> view_space_position{ *Perceiver::Instance->GetInverseProjectionMatrix() * Vector4<float>(Vector3<float>(near_plane_coordinate, depth), 1.0f) };
		const float inverse_view_space_position_denominator{ 1.0f / view_space_position._W };
		view_space_position *= inverse_view_space_position_denominator;
		const Vector4<float> world_space_position{ *Perceiver::Instance->GetInversePerceiverMatrix() * view_space_position };

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
	FORCE_INLINE static void GenerateMipChain(const Texture2D<TYPE> &input_texture, const uint8 levels, DynamicArray<Texture2D<TYPE>> *const RESTRICT output_textures) NOEXCEPT
	{
		//Upsize the output textures accordingly.
		output_textures->Upsize<true>(levels);

		//Copy the original mip level.
		output_textures->At(0) = input_texture;

		//Generate all mip levels.
		for (uint8 i{ 1 }; i < levels; ++i)
		{
			//Initialize this mip level.
			output_textures->At(i).Initialize(output_textures->At(i - 1).GetWidth() >> 1, output_textures->At(i - 1).GetHeight() >> 1);

			for (uint32 Y{ 0 }; Y < output_textures->At(i).GetHeight(); ++Y)
			{
				for (uint32 X{ 0 }; X < output_textures->At(i).GetWidth(); ++X)
				{
					//Calculate the normalized coordinate.
					const Vector2<float32> normalized_coordinate{ (static_cast<float32>(X) + 0.5f) / static_cast<float32>(output_textures->At(i).GetWidth()), (static_cast<float32>(Y) + 0.5f) / static_cast<float32>(output_textures->At(i).GetHeight()) };

					//Retrieve the value at this texel.
					output_textures->At(i).At(X, Y) = output_textures->At(i - 1).Sample(normalized_coordinate, AddressMode::ClampToEdge);
				}
			}
		}
	}

	/*
	*	Returns whether or not an axis-aligned bounding box is within the view frustum.
	*/
	FORCE_INLINE static bool IsWithinViewFrustum(const StaticArray<Vector4<float>, 6> &planes, const AxisAlignedBoundingBox3 &box) NOEXCEPT
	{
		for (uint8 i = 0; i < 6; ++i)
		{
			Vector3<float> axis;

			if (planes[i]._X < 0.0f)
			{
				axis._X = box._Minimum._X;
			}

			else
			{
				axis._X = box._Maximum._X;
			}

			if (planes[i]._Y < 0.0f)
			{
				axis._Y = box._Minimum._Y;
			}

			else
			{
				axis._Y = box._Maximum._Y;
			}

			if (planes[i]._Z < 0.0f)
			{
				axis._Z = box._Minimum._Z;
			}

			else
			{
				axis._Z = box._Maximum._Z;
			}

			const Vector3<float> planeNormal{ Vector3<float>(planes[i]._X, planes[i]._Y, planes[i]._Z) };

			if (Vector3<float>::DotProduct(planeNormal, axis) + planes[i]._W < 0.0f)
			{
				return false;
			}
		}

		return true;
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
	FORCE_INLINE static void TransformAxisAlignedBoundingBox(const AxisAlignedBoundingBox3 &originalBox, const Matrix4x4 &transformation, AxisAlignedBoundingBox3 *const RESTRICT newBox) NOEXCEPT
	{
		const Vector3<float> xMinimum{ transformation.GetRight() * originalBox._Minimum._X };
		const Vector3<float> xMaximum{ transformation.GetRight() * originalBox._Maximum._X };

		const Vector3<float> yMinimum{ transformation.GetUp() * originalBox._Minimum._Y };
		const Vector3<float> yMaximum{ transformation.GetUp() * originalBox._Maximum._Y };

		const Vector3<float> zMinimum{ transformation.GetForward() * originalBox._Minimum._Z };
		const Vector3<float> zMaximum{ transformation.GetForward() * originalBox._Maximum._Z };

		newBox->_Minimum = transformation.GetTranslation() + Vector3<float>::Minimum(xMinimum, xMaximum) + Vector3<float>::Minimum(yMinimum, yMaximum) + Vector3<float>::Minimum(zMinimum, zMaximum);
		newBox->_Maximum = transformation.GetTranslation() + Vector3<float>::Maximum(xMinimum, xMaximum) + Vector3<float>::Maximum(yMinimum, yMaximum) + Vector3<float>::Maximum(zMinimum, zMaximum);
	}

};