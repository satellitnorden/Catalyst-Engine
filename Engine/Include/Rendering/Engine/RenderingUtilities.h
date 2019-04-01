#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace RenderingUtilities
{

	/*
	*	Calculates an axis-aligned bounding box from a set of transformations.
	*/
	static void CalculateAxisAlignedBoundingBoxFromTransformations(const DynamicArray<Matrix4> &transformations, const AxisAlignedBoundingBox &modelBox, AxisAlignedBoundingBox *const RESTRICT box) NOEXCEPT
	{
		float extent{ CatalystBaseMath::Absolute(modelBox._Minimum._X) };
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(modelBox._Minimum._Y));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(modelBox._Minimum._Z));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(modelBox._Maximum._X));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(modelBox._Maximum._Y));
		extent = CatalystBaseMath::Maximum<float>(extent, CatalystBaseMath::Absolute(modelBox._Maximum._Z));

		extent *= 2.0f;

		box->_Minimum._X = FLOAT_MAXIMUM;
		box->_Minimum._Y = FLOAT_MAXIMUM;
		box->_Minimum._Z = FLOAT_MAXIMUM;

		box->_Maximum._X = -FLOAT_MAXIMUM;
		box->_Maximum._Y = -FLOAT_MAXIMUM;
		box->_Maximum._Z = -FLOAT_MAXIMUM;

		float biggestScale{ -FLOAT_MAXIMUM };

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

			biggestScale = CatalystBaseMath::Maximum<float>(biggestScale, scale._X);
			biggestScale = CatalystBaseMath::Maximum<float>(biggestScale, scale._Y);
			biggestScale = CatalystBaseMath::Maximum<float>(biggestScale, scale._Z);
		}

		box->_Minimum._X -= extent * biggestScale;
		box->_Minimum._Y -= extent * biggestScale;
		box->_Minimum._Z -= extent * biggestScale;

		box->_Maximum._X += extent * biggestScale;
		box->_Maximum._Y += extent * biggestScale;
		box->_Maximum._Z += extent * biggestScale;
	}

	/*
	*	Creates a transformations buffer.
	*/
	static void CreateTransformationsBuffer(const DynamicArray<Matrix4> &transformations, ConstantBufferHandle *const RESTRICT buffer) NOEXCEPT
	{
		const void *RESTRICT data[]{ transformations.Data() };
		const uint64 dataSizes[]{ sizeof(Matrix4) * transformations.Size() };
		*buffer = RenderingSystem::Instance->CreateBuffer(dataSizes[0]);
		RenderingSystem::Instance->UploadDataToBuffer(data, dataSizes, 1, *buffer);
	}

	/*
	*	Returns whether or not an axis-aligned bounding box is within the view frustum.
	*/
	static bool IsWithinViewFrustum(const StaticArray<Vector4<float>, 6> &planes, const AxisAlignedBoundingBox &box) NOEXCEPT
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
}