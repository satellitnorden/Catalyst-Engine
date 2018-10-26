#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainVertex.h>

namespace TerrainUtilities
{

	/*
	*	Finds the minimum and the maximum height values given a CPU texture 2D.
	*/
	static void FindMinimumMaximumHeight(const DynamicArray<TerrainVertex> &vertices, float *const RESTRICT minimum, float *const RESTRICT maximum) NOEXCEPT
	{
		*minimum = FLOAT_MAXIMUM;
		*maximum = -FLOAT_MAXIMUM;

		for (const TerrainVertex &vertex : vertices)
		{
			*minimum = CatalystBaseMath::Minimum<float>(*minimum, vertex._PositionY);
			*maximum = CatalystBaseMath::Maximum<float>(*maximum, vertex._PositionY);
		}
	}

	/*
	*	Generates a normal at the given position.
	*/
	static void GenerateNormal(const TerrainProperties &properties, const Vector3 &position, Vector3 *const RESTRICT normal) NOEXCEPT
	{
		const float offset{ properties._PatchSize / properties._PatchResolution };

		Vector3 left{ position._X - offset, 0.0f, position._Z };
		properties._HeightGenerationFunction(properties, left, &left._Y);
		Vector3 right{ position._X + offset, 0.0f, position._Z };
		properties._HeightGenerationFunction(properties, right, &right._Y);
		Vector3 up{ position._X, 0.0f, position._Z - offset };
		properties._HeightGenerationFunction(properties, up, &up._Y);
		Vector3 down{ position._X, 0.0f, position._Z + offset };
		properties._HeightGenerationFunction(properties, down, &down._Y);
		Vector3 center{ position._X, 0.0f, position._Z };
		properties._HeightGenerationFunction(properties, center, &center._Y);

		const Vector3 normal1{ Vector3::CrossProduct(up - center, left - center) };
		const Vector3 normal2{ Vector3::CrossProduct(right - center, up - center) };
		const Vector3 normal3{ Vector3::CrossProduct(down - center, right - center) };
		const Vector3 normal4{ Vector3::CrossProduct(left - center, down - center) };

		*normal = Vector3::Normalize(normal1 + normal2 + normal3 + normal4);
	}

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	static void GenerateTerrainPlane(const TerrainProperties &properties, const uint32 resolution, const Vector3 &worldPosition, const float patchSize, const TerrainBorder borders, DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve((resolution + 1) * (resolution + 1) * 5);
		indices->Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				float textureCoordinateX;
				float textureCoordinateY;

				if ((j == 0 && TEST_BIT(borders, TerrainBorder::Upper))
					|| (j == resolution && TEST_BIT(borders, TerrainBorder::Lower)))
				{
					if (i % 3 == 0)
					{
						textureCoordinateX = static_cast<float>(i) / static_cast<float>(resolution);
					}

					else if ((i - 1) % 3 == 0)
					{
						textureCoordinateX = static_cast<float>(i - 1) / static_cast<float>(resolution);
					}

					else
					{
						textureCoordinateX = static_cast<float>(i - 2) / static_cast<float>(resolution);
					}
				}

				else
				{
					textureCoordinateX = static_cast<float>(i) / static_cast<float>(resolution);
				}

				if ((i == 0 && TEST_BIT(borders, TerrainBorder::Left))
					|| (i == resolution && TEST_BIT(borders, TerrainBorder::Right)))
				{
					if (j % 3 == 0)
					{
						textureCoordinateY = static_cast<float>(j) / static_cast<float>(resolution);
					}

					else if ((j - 1) % 3 == 0)
					{
						textureCoordinateY = static_cast<float>(j - 1) / static_cast<float>(resolution);
					}

					else
					{
						textureCoordinateY = static_cast<float>(j - 2) / static_cast<float>(resolution);
					}
				}

				else
				{
					textureCoordinateY = static_cast<float>(j) / static_cast<float>(resolution);
				}

				TerrainVertex vertex;

				vertex._PositionX = worldPosition._X + ((-1.0f + (2.0f * textureCoordinateX)) * (patchSize * 0.5f));
				vertex._PositionZ = worldPosition._Z + ((-1.0f + (2.0f * textureCoordinateY)) * (patchSize * 0.5f));
				properties._HeightGenerationFunction(properties, Vector3(vertex._PositionX, 0.0f, vertex._PositionZ), &vertex._PositionY);

				const Vector3 worldPosition{ vertex._PositionX, vertex._PositionY, vertex._PositionZ };
				Vector3 normal;

				GenerateNormal(properties, worldPosition, &normal);

				vertex._NormalX = normal._X;
				vertex._NormalY = normal._Y;
				vertex._NormalZ = normal._Z;

				Vector4 layerWeights;

				properties._LayerWeightsGenerationFunction(properties, worldPosition, normal, &layerWeights);

				vertex._LayerWeightX = layerWeights._X;
				vertex._LayerWeightY = layerWeights._Y;
				vertex._LayerWeightZ = layerWeights._Z;
				vertex._LayerWeightW = layerWeights._W;

				vertices->EmplaceFast(vertex);

				if (i != resolution && j != resolution)
				{
					indices->EmplaceFast((i * (resolution + 1)) + j);
					indices->EmplaceFast((i * (resolution + 1)) + j + 1);
					indices->EmplaceFast(((i + 1) * (resolution + 1)) + j);

					indices->EmplaceFast((i * (resolution + 1)) + j + 1);
					indices->EmplaceFast(((i + 1) * (resolution + 1)) + j + 1);
					indices->EmplaceFast(((i + 1) * (resolution + 1)) + j);
				}
			}
		}
	}

}