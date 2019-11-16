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
	*	Generates a normal at the given position.
	*/
	FORCE_INLINE static void GenerateNormal(const TerrainProperties &properties, const Vector3<float> &position, Vector3<float> *const RESTRICT normal, float *const RESTRICT height = nullptr, const void *const RESTRICT context = nullptr) NOEXCEPT
	{
		constexpr float OFFSET{ 1.0f };

		float center;
		float up;
		float right;

		properties._HeightFunction(properties, position, &center, context);
		properties._HeightFunction(properties, position + Vector3<float>(0.0f, 0.0f, OFFSET), &up, context);
		properties._HeightFunction(properties, position + Vector3<float>(OFFSET, 0.0f, 0.0f), &right, context);
		
		normal->_X = center - right;
		normal->_Y = 1.0f;
		normal->_Z = center - up;

		normal->Normalize();

		if (height)
		{
			*height = center;
		}
	}

	/*
	*	Generates a height texture.
	*/
	FORCE_INLINE static void GenerateHeightTexture(const TerrainProperties &properties, const float patchSizeMultiplier, const Vector3<float> &patchWorldPosition, float *const RESTRICT minimumHeight, float *const RESTRICT maximumHeight, Texture2DHandle *const RESTRICT texture) NOEXCEPT
	{
		*minimumHeight = FLOAT_MAXIMUM;
		*maximumHeight = -FLOAT_MAXIMUM;

		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };

		DynamicArray<float> data;
		data.UpsizeFast(TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION * TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION);

		for (uint32 i = 0; i < TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION; ++i)
		{
			for (uint32 j = 0; j < TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION; ++j)
			{
				const float coordinateX{ static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION - 1) };
				const float coordinateY{ static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION - 1) };

				const Vector3<float> worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
													0.0f,
													patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				float height;

				properties._HeightFunction(properties, worldPosition, &height, nullptr);

				data[(j * TerrainConstants::TERRAIN_PATCH_RESOLUTION) + i] = height;

				*minimumHeight = CatalystBaseMath::Minimum<float>(*minimumHeight, height);
				*maximumHeight = CatalystBaseMath::Maximum<float>(*maximumHeight, height);
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(	TextureDataContainer(data.Data(),
																TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION,
																TerrainConstants::TERRAIN_HEIGHT_TEXTURE_RESOLUTION,
																1,
																1),
																TextureFormat::R32_Float), texture);
	}

	/*
	*	Generates the terrain textures.
	*/
	FORCE_INLINE static void GenerateTerrainTextures(const TerrainProperties &properties, const float patchSizeMultiplier, const uint32 resolution, const Vector3<float> &patchWorldPosition, Texture2DHandle *const RESTRICT normalTextureHandle, Texture2DHandle *const RESTRICT materialTextureHandle) NOEXCEPT
	{
		//Calculate the patch size.
		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };

		//Store the intermediate data in a CPU texture.
		Texture2D<Vector4<byte>> normalTexture{ resolution };
		Texture2D<byte> materialTexture{ resolution };

		for (uint32 i = 0; i < resolution; ++i)
		{
			for (uint32 j = 0; j < resolution; ++j)
			{
				const float coordinateX{ static_cast<float>(i) / static_cast<float>(resolution - 1) };
				const float coordinateY{ static_cast<float>(j) / static_cast<float>(resolution - 1) };
			
				const Vector3<float> worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
													0.0f,
													patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				//Generate the height and normal.
				float height;
				Vector3<float> normal;
				GenerateNormal(properties, worldPosition, &normal, &height);

				//Write the normal.
				Vector4<byte> &normalTextureValue{ normalTexture.At(i, j) };

				normalTextureValue._X = static_cast<byte>(((normal._X + 1.0f) * 0.5f) * 255.0f);
				normalTextureValue._Y = static_cast<byte>(((normal._Y + 1.0f) * 0.5f) * 255.0f);
				normalTextureValue._Z = static_cast<byte>(((normal._Z + 1.0f) * 0.5f) * 255.0f);
				normalTextureValue._W = 255;

				//Write the material.
				properties._MaterialFunction(properties, worldPosition, height, normal, &materialTexture.At(i, j), nullptr);
			}
		}

		//Create the textures.
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(normalTexture), TextureFormat::R8G8B8A8_Byte), normalTextureHandle);
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(materialTexture), TextureFormat::R8_Byte), materialTextureHandle);
	}

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
	FORCE_INLINE static void GenerateTerrainPlane(DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
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

				vertex._Position._X = static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) - 0.5f;
				vertex._Position._Y = static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_PATCH_RESOLUTION - 1) - 0.5f;

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