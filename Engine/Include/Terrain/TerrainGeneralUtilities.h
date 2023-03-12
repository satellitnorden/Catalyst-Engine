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
	FORCE_INLINE static void GenerateTerrainPlane(const Texture2D<float32> &heightmap, DynamicArray<TerrainVertex>* const RESTRICT vertices, DynamicArray<uint32>* const RESTRICT indices) NOEXCEPT
	{
		const uint32 resolution{ heightmap.GetResolution() };
		const uint32 half_resolution{ resolution / 2 };

		vertices->Reserve(resolution * resolution);
		indices->Reserve((resolution - 1) * (resolution - 1) * 6);

		for (uint32 Y{ 0 }; Y < resolution; ++Y)
		{
			for (uint32 X{ 0 }; X < resolution; ++X)
			{
				TerrainVertex vertex;

				vertex._Position._X = CatalystBaseMath::LinearlyInterpolate(-static_cast<float32>(half_resolution), static_cast<float32>(half_resolution), static_cast<float32>(X) / static_cast<float32>(resolution - 1));
				vertex._Position._Y = heightmap.At(X, Y);
				vertex._Position._Z = CatalystBaseMath::LinearlyInterpolate(-static_cast<float32>(half_resolution), static_cast<float32>(half_resolution), static_cast<float32>(Y) / static_cast<float32>(resolution - 1));

				vertex._TextureCoordinate._X = static_cast<float32>(X) / static_cast<float32>(resolution - 1);
				vertex._TextureCoordinate._Y = static_cast<float32>(Y) / static_cast<float32>(resolution - 1);

				vertices->Emplace(vertex);

				if (X != resolution - 1 && Y != resolution - 1)
				{
					indices->Emplace(X + (Y * resolution));
					indices->Emplace(X + ((Y + 1) * resolution));
					indices->Emplace((X + 1) + (Y * resolution));

					indices->Emplace((X + 1) + (Y * resolution));
					indices->Emplace(X + ((Y + 1) * resolution));
					indices->Emplace((X + 1) + ((Y + 1) * resolution));
				}
			}
		}
	}

}