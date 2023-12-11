//Header file.
#include <Terrain/TerrainGeneralUtilities.h>

namespace TerrainGeneralUtilities
{

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	void GenerateTerrainPlane(const uint32 resolution, DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve(resolution * resolution);
		indices->Reserve((resolution - 1) * (resolution - 1) * 6);

		for (uint32 Y{ 0 }; Y < resolution; ++Y)
		{
			for (uint32 X{ 0 }; X < resolution; ++X)
			{
				const Vector2<float32> heightmap_coordinate{ static_cast<float32>(X) / static_cast<float32>(resolution - 1), static_cast<float32>(Y) / static_cast<float32>(resolution - 1) };

				TerrainVertex vertex;

				vertex._Position._X = static_cast<float32>(X) / static_cast<float32>(resolution - 1);
				vertex._Position._Y = static_cast<float32>(Y) / static_cast<float32>(resolution - 1);

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