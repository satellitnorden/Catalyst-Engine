#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector4.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalVertex.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>
#include <Rendering/Engine Layer/WaterUniformData.h>

namespace RenderingUtilities
{
	/*
	*	Given terrain properties, terrain uniform data and a resolution, generate terrain plane vertices and indices.
	*/
	static void GenerateTerrainPlane(const CPUTexture4 &terrainProperties, const TerrainUniformData &terrainUniformData, const uint32 resolution, DynamicArray<float> &vertices, DynamicArray<uint32> &indices) NOEXCEPT
	{
		vertices.Reserve((resolution + 1) * (resolution + 1) * 5);
		indices.Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				const float xTextureCoordinate{ StaticCast<float>(i) / StaticCast<float>(resolution) };
				const float yTextureCoordinate{ StaticCast<float>(j) / StaticCast<float>(resolution) };

				vertices.EmplaceFast((-1.0f + (2.0f * StaticCast<float>(i) / StaticCast<float>(resolution))) * (terrainUniformData.terrainSize * 0.5f) + terrainUniformData.terrainPosition.X);
				vertices.EmplaceFast(terrainProperties.At(xTextureCoordinate, yTextureCoordinate).W * terrainUniformData.terrainHeight);
				vertices.EmplaceFast((-1.0f + (2.0f * StaticCast<float>(j) / StaticCast<float>(resolution))) * (terrainUniformData.terrainSize * 0.5f) + terrainUniformData.terrainPosition.Z);
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
	*	Given a resolution and the water uniform data, generate water vertices and indices.
	*/
	static void GenerateWaterPlane(const uint32 resolution, const WaterUniformData waterUniformData, DynamicArray<float> &vertices, DynamicArray<uint32> &indices) NOEXCEPT
	{
		vertices.Reserve((resolution + 1) * (resolution + 1) * 5);
		indices.Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				const float xTextureCoordinate{ StaticCast<float>(i) / StaticCast<float>(resolution) };
				const float yTextureCoordinate{ StaticCast<float>(j) / StaticCast<float>(resolution) };

				vertices.EmplaceFast((-1.0f + (2.0f * StaticCast<float>(i) / StaticCast<float>(resolution))) * (waterUniformData.waterSize * 0.5f) + waterUniformData.waterWorldPosition.X);
				vertices.EmplaceFast(0.0f);
				vertices.EmplaceFast((-1.0f + (2.0f * StaticCast<float>(j) / StaticCast<float>(resolution))) * (waterUniformData.waterSize * 0.5f) + waterUniformData.waterWorldPosition.Z);
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
	static bool IsCubeWithinViewFrustum(const Vector4 *RESTRICT corners) NOEXCEPT
	{
		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = CatalystMath::Maximum(highestX, corners[i].X);
			lowestX = CatalystMath::Minimum(lowestX, corners[i].X);
			highestY = CatalystMath::Maximum(highestY, corners[i].Y);
			lowestY = CatalystMath::Minimum(lowestY, corners[i].Y);
			highestZ = CatalystMath::Maximum(highestZ, corners[i].Z);
			lowestZ = CatalystMath::Minimum(lowestZ, corners[i].Z);
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