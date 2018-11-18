#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/TextureData.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainVertex.h>

namespace TerrainGeneralUtilities
{
	/*
	*	Generates a normal at the given position.
	*/
	static void GenerateNormal(const TerrainProperties &properties, const Vector3 &position, Vector3 *const RESTRICT normal) NOEXCEPT
	{
		constexpr float offset{ 1.0f };

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
	*	Generates a height texture.
	*/
	static void GenerateHeightTexture(const TerrainProperties &properties, const float patchSizeMultiplier, const Vector3 &patchWorldPosition, float *const RESTRICT minimumHeight, float *const RESTRICT maximumHeight, Texture2DHandle *const RESTRICT texture) NOEXCEPT
	{
		*minimumHeight = FLOAT_MAXIMUM;
		*maximumHeight = -FLOAT_MAXIMUM;

		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };

		DynamicArray<float> data;
		data.UpsizeFast(TerrainConstants::TERRAIN_PATCH_RESOLUTION * TerrainConstants::TERRAIN_PATCH_RESOLUTION);

		for (uint32 i = 0; i < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++i)
		{
			for (uint32 j = 0; j < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++j)
			{
				const float coordinateX{ static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) };
				const float coordinateY{ static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) };

				const Vector3 worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
												0.0f,
												patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				float height;

				properties._HeightGenerationFunction(properties, worldPosition, &height);

				data[((j * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + i)] = height;

				*minimumHeight = CatalystBaseMath::Minimum<float>(*minimumHeight, height);
				*maximumHeight = CatalystBaseMath::Maximum<float>(*maximumHeight, height);
			}
		}

		*texture = RenderingSystem::Instance->CreateTexture2D(	TextureData(	TextureDataContainer(data.Data(),
																				TerrainConstants::TERRAIN_PATCH_RESOLUTION,
																				TerrainConstants::TERRAIN_PATCH_RESOLUTION,
																				1),
																				TextureFormat::R32_Float));
	}

	/*
	*	Generates a normal texture.
	*/
	static void GenerateNormalTexture(const TerrainProperties &properties, const float patchSizeMultiplier, const uint8 resolutionMultiplier, const Vector3 &patchWorldPosition, Texture2DHandle *const RESTRICT texture) NOEXCEPT
	{
		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };
		const uint32 resolution{ TerrainConstants::TERRAIN_PATCH_RESOLUTION * resolutionMultiplier };

		DynamicArray<byte> data;
		data.UpsizeFast(resolution * resolution * 4);

		for (uint32 i = 0; i < resolution; ++i)
		{
			for (uint32 j = 0; j < resolution; ++j)
			{
				const float coordinateX{ static_cast<float>(i) / static_cast<float>(resolution - 1) };
				const float coordinateY{ static_cast<float>(j) / static_cast<float>(resolution - 1) };
			
				const Vector3 worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
												0.0f,
												patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				Vector3 normal;

				GenerateNormal(properties, worldPosition, &normal);

				data[((j * resolution) + i) * 4] = static_cast<byte>(((normal._X + 1.0f) * 0.5f) * 255.0f);
				data[((j * resolution) + i) * 4 + 1] = static_cast<byte>(((normal._Y + 1.0f) * 0.5f) * 255.0f);
				data[((j * resolution) + i) * 4 + 2] = static_cast<byte>(((normal._Z + 1.0f) * 0.5f) * 255.0f);
				data[((j * resolution) + i) * 4 + 3] = 255;
			}
		}

		*texture = RenderingSystem::Instance->CreateTexture2D(TextureData(	TextureDataContainer(	data.Data(),
																									resolution,
																									resolution,
																									4),
																			TextureFormat::R8G8B8A8_Byte));
	}

	/*
	*	Generates a layer weights texture.
	*/
	static void GenerateLayerWeightsTexture(const TerrainProperties &properties, const float patchSizeMultiplier, const Vector3 &patchWorldPosition, Texture2DHandle *const RESTRICT texture) NOEXCEPT
	{
		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };
		DynamicArray<byte> data;
		data.UpsizeFast(TerrainConstants::TERRAIN_PATCH_RESOLUTION * TerrainConstants::TERRAIN_PATCH_RESOLUTION * 4);

		for (uint32 i = 0; i < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++i)
		{
			for (uint32 j = 0; j < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++j)
			{
				const float coordinateX{ static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) };
				const float coordinateY{ static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) };

				const Vector3 worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
												0.0f,
												patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				Vector4 layerWeights;

				properties._LayerWeightsGenerationFunction(properties, worldPosition, &layerWeights);

				data[((j * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + i) * 4] = static_cast<byte>(layerWeights._X * 255.0f);
				data[((j * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + i) * 4 + 1] = static_cast<byte>(layerWeights._Y * 255.0f);
				data[((j * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + i) * 4 + 2] = static_cast<byte>(layerWeights._Z * 255.0f);
				data[((j * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + i) * 4 + 3] = static_cast<byte>(layerWeights._W * 255.0f);
			}
		}

		*texture = RenderingSystem::Instance->CreateTexture2D(	TextureData(	TextureDataContainer(data.Data(),
																				TerrainConstants::TERRAIN_PATCH_RESOLUTION,
																				TerrainConstants::TERRAIN_PATCH_RESOLUTION,
																				4),
																TextureFormat::R8G8B8A8_Byte));
	}

	/*
	*	Generates a patch identifier.
	*/
	static uint64 GeneratePatchIdentifier() NOEXCEPT
	{
		static uint64 counter{ 0 };

		return counter++;
	}

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	static void GenerateTerrainPlane(DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve(TerrainConstants::TERRAIN_PATCH_RESOLUTION * TerrainConstants::TERRAIN_PATCH_RESOLUTION);
		indices->Reserve((TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) * (TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) * 6);

		uint32 x{ 0 };
		uint32 y{ 0 };

		for (uint32 i{ 0 }; i < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++i, x = x < 3 ? x + 1 : 0)
		{
			for (uint32 j{ 0 }; j < TerrainConstants::TERRAIN_PATCH_RESOLUTION; ++j, y = y < 3 ? y + 1 : 0)
			{
				TerrainVertex vertex;

				vertex._PositionX = static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) - 0.5f;
				vertex._PositionZ = static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) - 0.5f;

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
				else if (i == TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1)
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
				else if (j == TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1)
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

				if (i != TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1 && j != TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1)
				{
					indices->EmplaceFast((i * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j);
					indices->EmplaceFast((i * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j + 1);
					indices->EmplaceFast(((i + 1) * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j);

					indices->EmplaceFast((i * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j + 1);
					indices->EmplaceFast(((i + 1) * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j + 1);
					indices->EmplaceFast(((i + 1) * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + j);
				}
			}
		}
	}
}