#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace RenderingUtilities
{

	/*
	*	Calculates an axis-aligned bounding box from a set of transformations.
	*/
	FORCE_INLINE static void CalculateAxisAlignedBoundingBoxFromTransformations(const DynamicArray<Matrix4> &transformations, const AxisAlignedBoundingBox &model_space_bounding_box, AxisAlignedBoundingBox *const RESTRICT box) NOEXCEPT
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

		for (const Matrix4 &transformation : transformations)
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
	*	Calculates the screen coordinate of a position.
	*/
	FORCE_INLINE static Vector2<float> CalculateScreenCoordinate(const Matrix4 &view_matrix, const Vector3<float> &position) NOEXCEPT
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
	*	Calculates the screen coverage percent of an axis aligned bounding box.
	*/
	FORCE_INLINE static float CalculateScreenCoveragePercent(const Matrix4 &view_matrix, const AxisAlignedBoundingBox &box) NOEXCEPT
	{
		Vector2<float> screen_minimum{ FLOAT_MAXIMUM, FLOAT_MAXIMUM };
		Vector2<float> screen_maximum{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

		StaticArray<Vector3<float>, 8> corners;

		const Vector3<float> difference{ box._Maximum - box._Minimum };

		corners[0] = box._Minimum + difference * Vector3<float>(0.0f, 0.0f, 0.0f);
		corners[1] = box._Minimum + difference * Vector3<float>(0.0f, 0.0f, 1.0f);
		corners[2] = box._Minimum + difference * Vector3<float>(0.0f, 1.0f, 0.0f);
		corners[3] = box._Minimum + difference * Vector3<float>(0.0f, 1.0f, 1.0f);
		corners[4] = box._Minimum + difference * Vector3<float>(1.0f, 0.0f, 0.0f);
		corners[5] = box._Minimum + difference * Vector3<float>(1.0f, 0.0f, 1.0f);
		corners[6] = box._Minimum + difference * Vector3<float>(1.0f, 1.0f, 0.0f);
		corners[7] = box._Minimum + difference * Vector3<float>(1.0f, 1.0f, 1.0f);

		for (const Vector3<float> &corner : corners)
		{
			const Vector2<float> corner_screen_coordinate{ CalculateScreenCoordinate(view_matrix, corner) };

			screen_minimum = Vector2<float>::Minimum(screen_minimum, corner_screen_coordinate);
			screen_maximum = Vector2<float>::Maximum(screen_maximum, corner_screen_coordinate);
		}

		screen_minimum._X = CatalystBaseMath::Clamp<float>(screen_minimum._X, 0.0f, 1.0f);
		screen_minimum._Y = CatalystBaseMath::Clamp<float>(screen_minimum._Y, 0.0f, 1.0f);

		screen_maximum._X = CatalystBaseMath::Clamp<float>(screen_maximum._X, 0.0f, 1.0f);
		screen_maximum._Y = CatalystBaseMath::Clamp<float>(screen_maximum._Y, 0.0f, 1.0f);

		return (screen_maximum._X - screen_minimum._X) * (screen_maximum._Y - screen_minimum._Y);
	}

	/*
	*	Creates a transformations buffer.
	*/
	FORCE_INLINE static void CreateTransformationsBuffer(const DynamicArray<Matrix4> &transformations, BufferHandle *const RESTRICT buffer) NOEXCEPT
	{
		const void *RESTRICT data[]{ transformations.Data() };
		const uint64 dataSizes[]{ sizeof(Matrix4) * transformations.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, buffer);
		RenderingSystem::Instance->UploadDataToBuffer(data, dataSizes, 1, buffer);
	}

	/*
	*	Returns whether or not an axis-aligned bounding box is within the view frustum.
	*/
	FORCE_INLINE static bool IsWithinViewFrustum(const StaticArray<Vector4<float>, 6> &planes, const AxisAlignedBoundingBox &box) NOEXCEPT
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
	*	Transforms an axis aligned bounding box from one space to another.
	*/
	FORCE_INLINE static void TransformAxisAlignedBoundingBox(const AxisAlignedBoundingBox &originalBox, const Matrix4 &transformation, AxisAlignedBoundingBox *const RESTRICT newBox) NOEXCEPT
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

}