#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Matrix4.h>
#include <Math/Vector4.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TerrainUniformData.h>

//Resources.
#include <Resources/PhysicalModelData.h>

namespace RenderingUtilities
{

	/*
	*	Calculates the directional light view matrix.
	*/
	static Matrix4 CalculateDirectionalLightViewMatrix() NOEXCEPT
	{
		static Matrix4 directionalLightProjectionMatrix{ Matrix4::Ortographic(-RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, -RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, 0.0f, RenderingConstants::SHADOW_VIEW_DISTANCE) };

		const CameraComponent *const RESTRICT cameraComponent{ ComponentManager::GetCameraCameraComponents() };
		const DirectionalLightComponent *const RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightDirectionalLightComponents() };

		const Vector3 cameraPosition{ cameraComponent->_Position };
		const Vector3 directionalLightForwardVector{ Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->_Rotation) };
		const Vector3 directionalLightUpVector{ Vector3(0.0f, 1.0f, 0.0f).Rotated(directionalLightComponent->_Rotation) };
		const Vector3 directionalLightPosition{ cameraPosition + (directionalLightForwardVector * -1.0f) * (RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f) };
		const Matrix4 directionalLightDirection{ Matrix4::LookAt(directionalLightPosition, directionalLightPosition + directionalLightForwardVector, directionalLightUpVector) };
	
		return directionalLightProjectionMatrix * directionalLightDirection;
	}

	/*
	*	Calculates an axis-aligned bounding box for a terrain.
	*/
	static void CalculateTerrainAxisAlignedBoundingBox(const CPUTexture2D *const RESTRICT properties, const float extent, const Vector3 *const RESTRICT _WorldPosition, const float height, AxisAlignedBoundingBox *const RESTRICT box) NOEXCEPT
	{
		float lowest{ FLOAT_MAXIMUM };
		float highest{ -FLOAT_MAXIMUM };

		for (const Vector4 &property : *properties)
		{
			lowest = CatalystBaseMath::Minimum(lowest, property._W);
			highest = CatalystBaseMath::Maximum(highest, property._W);
		}

		const float halfExtent{ extent * 0.5f };

		box->_Minimum = *_WorldPosition + Vector3(-halfExtent, lowest * height, -halfExtent);
		box->_Maximum = *_WorldPosition + Vector3(halfExtent, highest * height, halfExtent);
	}

	/*
	*	Given terrain properties, terrain uniform data and a resolution, generate terrain plane vertices and indices.
	*/
	static void GenerateTerrainPlane(const uint32 resolution, DynamicArray<float> &vertices, DynamicArray<uint32> &indices) NOEXCEPT
	{
		vertices.Reserve((resolution + 1) * (resolution + 1) * 5);
		indices.Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				const float xTextureCoordinate{ static_cast<float>(i) / static_cast<float>(resolution) };
				const float yTextureCoordinate{ static_cast<float>(j) / static_cast<float>(resolution) };

				vertices.EmplaceFast(-1.0f + (2.0f * xTextureCoordinate));
				vertices.EmplaceFast(0.0f);
				vertices.EmplaceFast(-1.0f + (2.0f * yTextureCoordinate));
				vertices.EmplaceFast(xTextureCoordinate);
				vertices.EmplaceFast(yTextureCoordinate);

				if (i != resolution && j != resolution)
				{
					indices.EmplaceFast((i * (resolution + 1)) + j);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices.EmplaceFast((i * (resolution + 1)) + j + 1);

					indices.EmplaceFast((i * (resolution + 1)) + j + 1);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j + 1);
				}
			}
		}
	}

	/*
	*	Given 8 corners of a cube, determine if it is within the view frustum.
	*/
	static bool IsCubeWithinViewFrustum(const StaticArray<Vector4, 8> &corners) NOEXCEPT
	{
		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = CatalystBaseMath::Maximum(highestX, corners[i]._X);
			lowestX = CatalystBaseMath::Minimum(lowestX, corners[i]._X);
			highestY = CatalystBaseMath::Maximum(highestY, corners[i]._Y);
			lowestY = CatalystBaseMath::Minimum(lowestY, corners[i]._Y);
			highestZ = CatalystBaseMath::Maximum(highestZ, corners[i]._Z);
			lowestZ = CatalystBaseMath::Minimum(lowestZ, corners[i]._Z);
		}

		if (((highestX > 1.0f && lowestX > 1.0f) || (highestX < -1.0f && lowestX < -1.0f))
			||
			((highestY > 1.0f && lowestY > 1.0f) || (highestY < -1.0f && lowestY < -1.0f))
			||
			((highestZ > 1.0f && lowestZ > 1.0f) || (highestZ < 0.0f && lowestZ < 0.0f)))
		{
			return false;
		}

		else
		{
			return true;
		}
	}

	/*
	*	Given a view matrix and an axis-aligned bounding box, returns if the box is in the view frustum.
	*/
	static bool IsInViewFrustum(const Matrix4 &viewMatrix, const AxisAlignedBoundingBox &axisAlignedBoundingBox) NOEXCEPT
	{
		StaticArray<Vector4, 8> corners;

		corners[0] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);
		corners[1] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);
		corners[2] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);
		corners[3] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);

		corners[4] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);
		corners[5] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);
		corners[6] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);
		corners[7] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);

		for (uint8 i = 0; i < 8; ++i)
		{
			corners[i] = viewMatrix * corners[i];

			const float inverseW{ 1.0f / corners[i]._W };

			corners[i]._X *= inverseW;
			corners[i]._Y *= inverseW;
			corners[i]._Z *= inverseW;
		}

		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = CatalystBaseMath::Maximum(highestX, corners[i]._X);
			lowestX = CatalystBaseMath::Minimum(lowestX, corners[i]._X);
			highestY = CatalystBaseMath::Maximum(highestY, corners[i]._Y);
			lowestY = CatalystBaseMath::Minimum(lowestY, corners[i]._Y);
			highestZ = CatalystBaseMath::Maximum(highestZ, corners[i]._Z);
			lowestZ = CatalystBaseMath::Minimum(lowestZ, corners[i]._Z);
		}

		if (((highestX > 1.0f && lowestX > 1.0f) || (highestX < -1.0f && lowestX < -1.0f))
			||
			((highestY > 1.0f && lowestY > 1.0f) || (highestY < -1.0f && lowestY < -1.0f))
			||
			((highestZ > 1.0f && lowestZ > 1.0f) || (highestZ < 0.0f && lowestZ < 0.0f)))
		{
			return false;
		}

		else
		{
			return true;
		}
	}
}