#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix3.h>

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
	static void GenerateNormal(const TerrainProperties &properties, const Vector3<float> &position, Vector3<float> *const RESTRICT normal) NOEXCEPT
	{
		constexpr float offset{ 1.0f };

		Vector3<float> left{ position._X - offset, 0.0f, position._Z };
		properties._HeightFunction(properties, left, &left._Y);
		Vector3<float> right{ position._X + offset, 0.0f, position._Z };
		properties._HeightFunction(properties, right, &right._Y);
		Vector3<float> up{ position._X, 0.0f, position._Z - offset };
		properties._HeightFunction(properties, up, &up._Y);
		Vector3<float> down{ position._X, 0.0f, position._Z + offset };
		properties._HeightFunction(properties, down, &down._Y);
		Vector3<float> center{ position._X, 0.0f, position._Z };
		properties._HeightFunction(properties, center, &center._Y);

		const Vector3<float> normal1{ Vector3<float>::CrossProduct(up - center, left - center) };
		const Vector3<float> normal2{ Vector3<float>::CrossProduct(right - center, up - center) };
		const Vector3<float> normal3{ Vector3<float>::CrossProduct(down - center, right - center) };
		const Vector3<float> normal4{ Vector3<float>::CrossProduct(left - center, down - center) };

		*normal = Vector3<float>::Normalize(normal1 + normal2 + normal3 + normal4);
	}

	/*
	*	Generates a height texture.
	*/
	static void GenerateHeightTexture(const TerrainProperties &properties, const float patchSizeMultiplier, const Vector3<float> &patchWorldPosition, float *const RESTRICT minimumHeight, float *const RESTRICT maximumHeight, Texture2DHandle *const RESTRICT texture) NOEXCEPT
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

				const Vector3<float> worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
													0.0f,
													patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				float height;

				properties._HeightFunction(properties, worldPosition, &height);

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
	*	Generates the material textures.
	*/
	static void GenerateMaterialTextures(const TerrainProperties &properties, const float patchSizeMultiplier, const Vector3<float> &patchWorldPosition, Texture2DHandle *const RESTRICT albedoTextureHandle, Texture2DHandle *const RESTRICT normalTextureHandle, Texture2DHandle *const RESTRICT materialPropertiesTextureHandle) NOEXCEPT
	{
		//Calculate the patch size.
		const float patchSize{ TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };

		//Store the intermediate data in CPU textures.
		CPUTexture2D<Vector4<byte>> albedoTexture{ TerrainConstants::TERRAIN_MATERIAL_RESOLUTION };
		CPUTexture2D<Vector4<byte>> normalTexture{ TerrainConstants::TERRAIN_MATERIAL_RESOLUTION };
		//CPUTexture2D<Vector4<byte>> materialProperties{ TerrainConstants::TERRAIN_MATERIAL_RESOLUTION };

		for (uint32 i = 0; i < TerrainConstants::TERRAIN_MATERIAL_RESOLUTION; ++i)
		{
			for (uint32 j = 0; j < TerrainConstants::TERRAIN_MATERIAL_RESOLUTION; ++j)
			{
				const float coordinateX{ static_cast<float>(i) / static_cast<float>(TerrainConstants::TERRAIN_MATERIAL_RESOLUTION - 1) };
				const float coordinateY{ static_cast<float>(j) / static_cast<float>(TerrainConstants::TERRAIN_MATERIAL_RESOLUTION - 1) };
			
				const Vector3<float> worldPosition{	patchWorldPosition._X + ((-1.0f + (2.0f * coordinateX)) * (patchSize * 0.5f)),
													0.0f,
													patchWorldPosition._Z + ((-1.0f + (2.0f * coordinateY)) * (patchSize * 0.5f)) };

				//Retrieve the height.
				float height;
				properties._HeightFunction(properties, worldPosition, &height);

				//Generate the normal.
				Vector3<float> normal;
				GenerateNormal(properties, worldPosition, &normal);

				//Generate the texture values.
				Vector4<byte> normalMapValue;
				properties._MaterialFunction(properties, worldPosition, height, normal, &albedoTexture.At(i, j), &normalMapValue);

				//Create the tangent space matrix.
				const Vector3<float> tangent{ Vector3<float>::CrossProduct(Vector3<float>::RIGHT, normal) };
				const Vector3<float> bitangent{ Vector3<float>::CrossProduct(tangent, normal) };

				const Matrix3 tangentSpaceMatrix{ tangent, bitangent, normal };

				//Convert the normal map to a Vector3 of floats.
				Vector3<float> normalMap{ static_cast<float>(normalMapValue._X) / 255.0f , static_cast<float>(normalMapValue._Y) / 255.0f , static_cast<float>(normalMapValue._Z) / 255.0f };
				normalMap = normalMap * 2.0f - 1.0f;

				//Apply the normal map.
				normal = Vector3<float>::Normalize(tangentSpaceMatrix * normalMap);

				//Write the normal.
				Vector4<byte> &normalTextureValue{ normalTexture.At(i, j) };

				normalTextureValue._X = static_cast<byte>(((normal._X + 1.0f) * 0.5f) * 255.0f);
				normalTextureValue._Y = static_cast<byte>(((normal._Y + 1.0f) * 0.5f) * 255.0f);
				normalTextureValue._Z = static_cast<byte>(((normal._Z + 1.0f) * 0.5f) * 255.0f);
				normalTextureValue._W = 255;
			}
		}

		//Create the textures.
		*albedoTextureHandle = RenderingSystem::Instance->CreateTexture2D(TextureData(	TextureDataContainer(albedoTexture),
																						TextureFormat::R8G8B8A8_Byte));

		*normalTextureHandle = RenderingSystem::Instance->CreateTexture2D(TextureData(	TextureDataContainer(normalTexture),
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