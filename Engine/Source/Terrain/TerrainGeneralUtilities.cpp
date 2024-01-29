//Header file.
#include <Terrain/TerrainGeneralUtilities.h>

//Core.
#include <Core/Containers/StaticArray.h>

namespace TerrainGeneralUtilities
{

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	void GenerateTerrainPlane(uint32 resolution, DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		constexpr StaticArray<bool, 4> FIRST_LAYER_STICTHING_MASK
		{
			false,
			true,
			false,
			true
		};

		constexpr StaticArray<bool, 4> SECOND_LAYER_STICTHING_MASK
		{
			false,
			false,
			true,
			true
		};

		++resolution;

		vertices->Reserve(resolution * resolution);
		indices->Reserve((resolution - 1) * (resolution - 1) * 6);

		for (uint32 Y{ 0 }; Y < resolution; ++Y)
		{
			for (uint32 X{ 0 }; X < resolution; ++X)
			{
				//Set up the vertex.
				TerrainVertex vertex;

				//Add the position.
				vertex._Position._X = static_cast<float32>(X) / static_cast<float32>(resolution - 1);
				vertex._Position._Y = static_cast<float32>(Y) / static_cast<float32>(resolution - 1);

				ASSERT(vertex._Position._X >= 0.0f && vertex._Position._X <= 1.0f && vertex._Position._Y >= 0.0f && vertex._Position._Y <= 1.0f, "oh no");

				//Add borders.
				vertex._Borders = 0;

				{
					//Left.
					if (X == 0 && Y != 0 && Y != (resolution - 1) && FIRST_LAYER_STICTHING_MASK[Y % FIRST_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(0);
					}

					if (X == 0 && Y != 0 && Y != (resolution - 1) && SECOND_LAYER_STICTHING_MASK[Y % SECOND_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(1);
					}

					//Right.
					if (X == (resolution - 1) && Y != 0 && Y != (resolution - 1) && FIRST_LAYER_STICTHING_MASK[Y % FIRST_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(2);
					}

					if (X == (resolution - 1) && Y != 0 && Y != (resolution - 1) && SECOND_LAYER_STICTHING_MASK[Y % SECOND_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(3);
					}

					//Down.
					if (Y == 0 && X != 0 && X != (resolution - 1) && FIRST_LAYER_STICTHING_MASK[X % FIRST_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(4);
					}

					if (Y == 0 && X != 0 && X != (resolution - 1) && SECOND_LAYER_STICTHING_MASK[X % SECOND_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(5);
					}

					//Up.
					if (Y == (resolution - 1) && X != 0 && X != (resolution - 1) && FIRST_LAYER_STICTHING_MASK[X % FIRST_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(6);
					}

					if (Y == (resolution - 1) && X != 0 && X != (resolution - 1) && SECOND_LAYER_STICTHING_MASK[X % SECOND_LAYER_STICTHING_MASK.Size()])
					{
						vertex._Borders |= BIT(7);
					}
				}

				vertices->Emplace(vertex);

				//Add indices.
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