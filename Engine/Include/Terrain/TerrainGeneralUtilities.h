#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/TextureData.h>
#include <Rendering/Native/Vertex.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainVertex.h>

namespace TerrainGeneralUtilities
{

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	FORCE_INLINE static void GenerateTerrainPlane(const TerrainProperties &properties, const uint32 resolution, DynamicArray<TerrainVertex>* const RESTRICT vertices, DynamicArray<uint32>* const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve(resolution * resolution);
		indices->Reserve((resolution - 1) * (resolution - 1) * 6);

		uint32 x{ 0 };
		uint32 y{ 0 };

		for (uint32 i{ 0 }; i < resolution; ++i, x = x < 3 ? x + 1 : 0)
		{
			for (uint32 j{ 0 }; j < resolution; ++j, y = y < 3 ? y + 1 : 0)
			{
				TerrainVertex vertex;

				vertex._Position._X = static_cast<float>(i) / static_cast<float>(resolution - 1) - 0.5f;
				vertex._Position._Y = static_cast<float>(j) / static_cast<float>(resolution - 1) - 0.5f;

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
				else if (i == resolution - 1)
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
				else if (j == resolution - 1)
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

				vertices->Emplace(vertex);

				if (i != resolution - 1 && j != resolution - 1)
				{
					indices->Emplace((i * resolution) + j);
					indices->Emplace((i * resolution) + j + 1);
					indices->Emplace(((i + 1) * resolution) + j);

					indices->Emplace((i * resolution) + j + 1);
					indices->Emplace(((i + 1) * resolution) + j + 1);
					indices->Emplace(((i + 1) * resolution) + j);
				}
			}
		}
	}

}