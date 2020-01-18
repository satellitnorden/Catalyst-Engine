#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/TextureData.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainVertex.h>

namespace TerrainGeneralUtilities
{
	/*
	*	Generates a patch identifier.
	*/
	FORCE_INLINE static NO_DISCARD uint64 GeneratePatchIdentifier() NOEXCEPT
	{
		static uint64 counter{ 0 };

		return counter++;
	}

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	FORCE_INLINE static void GenerateTerrainPlane(const TerrainProperties &properties, DynamicArray<TerrainVertex>* const RESTRICT vertices, DynamicArray<uint32>* const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve(properties._PatchResolution * properties._PatchResolution);
		indices->Reserve((properties._PatchResolution - 1) * (properties._PatchResolution - 1) * 6);

		uint32 x{ 0 };
		uint32 y{ 0 };

		for (uint32 i{ 0 }; i < properties._PatchResolution; ++i, x = x < 3 ? x + 1 : 0)
		{
			for (uint32 j{ 0 }; j < properties._PatchResolution; ++j, y = y < 3 ? y + 1 : 0)
			{
				TerrainVertex vertex;

				vertex._Position._X = static_cast<float>(i) / static_cast<float>(properties._PatchResolution - 1) - 0.5f;
				vertex._Position._Y = static_cast<float>(j) / static_cast<float>(properties._PatchResolution - 1) - 0.5f;

				vertex._Borders = 0;

				//Left.
				if (i == 0)
				{
					if (y == 1 || y == 3)
					{
						vertex._Borders |= BIT(6);
					}

					if (y == 2 || y == 3)
					{
						vertex._Borders |= BIT(7);
					}
				}

				//Right.
				else if (i == properties._PatchResolution - 1)
				{
					if (y == 1 || y == 3)
					{
						vertex._Borders |= BIT(2);
					}

					if (y == 2 || y == 3)
					{
						vertex._Borders |= BIT(3);
					}
				}

				//Upper.
				if (j == 0)
				{
					if (x == 1 || x == 3)
					{
						vertex._Borders |= BIT(0);
					}

					if (x == 2 || x == 3)
					{
						vertex._Borders |= BIT(1);
					}
				}

				//Lower.
				else if (j == properties._PatchResolution - 1)
				{
					if (x == 1 || x == 3)
					{
						vertex._Borders |= BIT(4);
					}

					if (x == 2 || x == 3)
					{
						vertex._Borders |= BIT(5);
					}
				}

				vertices->EmplaceFast(vertex);

				if (i != properties._PatchResolution - 1 && j != properties._PatchResolution - 1)
				{
					indices->EmplaceFast((i * properties._PatchResolution) + j);
					indices->EmplaceFast((i * properties._PatchResolution) + j + 1);
					indices->EmplaceFast(((i + 1) * properties._PatchResolution) + j);

					indices->EmplaceFast((i * properties._PatchResolution) + j + 1);
					indices->EmplaceFast(((i + 1) * properties._PatchResolution) + j + 1);
					indices->EmplaceFast(((i + 1) * properties._PatchResolution) + j);
				}
			}
		}
	}
}