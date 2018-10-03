#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Terrain.
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainVertex.h>

namespace TerrainUtilities
{

	/*
	*	Finds the minimum and the maximum height values given a CPU texture 2D.
	*/
	static void FindMinimumMaximumHeight(const CPUTexture2D &normalHeightMap, float *const RESTRICT minimum, float *const RESTRICT maximum) NOEXCEPT
	{
		*minimum = FLOAT_MAXIMUM;
		*maximum = -FLOAT_MAXIMUM;

		for (const Vector4 &normalHeight : normalHeightMap)
		{
			*minimum = CatalystBaseMath::Minimum<float>(*minimum, normalHeight._W);
			*maximum = CatalystBaseMath::Maximum<float>(*maximum, normalHeight._W);
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
	*	Generates the normals for a terrain patch.
	*/
	static void GeneratePatchNormals(const float size, CPUTexture2D *const RESTRICT properties) NOEXCEPT
	{
		const uint64 resolution{ properties->GetWidth() };

		const float offset{ size / static_cast<float>(resolution) };

		for (uint64 i = 0; i < resolution; ++i)
		{
			for (uint64 j = 0; j < resolution; ++j)
			{
				const Vector3 left{ -offset, properties->At(i > 0 ? i - 1 : i, j)._W, 0.0f };
				const Vector3 right{ offset, properties->At(i < resolution - 1 ? i + 1 : i, j)._W, 0.0f };
				const Vector3 up{ 0.0f, properties->At(i, j > 0 ? j - 1 : j)._W, -offset };
				const Vector3 down{ 0.0f, properties->At(i, j < resolution - 1 ? j + 1 : j)._W, offset };
				const Vector3 center{ 0.0f, properties->At(i, j)._W, 0.0f };

				const Vector3 normal1{ Vector3::CrossProduct(up - center, left - center) };
				const Vector3 normal2{ Vector3::CrossProduct(right - center, up - center) };
				const Vector3 normal3{ Vector3::CrossProduct(down - center, right - center) };
				const Vector3 normal4{ Vector3::CrossProduct(left - center, down - center) };

				Vector3 finalNormal{ Vector3::Normalize(normal1 + normal2 + normal3 + normal4) };
				
				Vector4 &property{ properties->At(i, j) };

				property._X = finalNormal._X;
				property._Y = finalNormal._Y;
				property._Z = finalNormal._Z;
			}
		}
	}

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	static void GenerateTerrainPlane(const TerrainProperties &properties, const uint32 resolution, const Vector3 &worldPosition, const CPUTexture2D &normalHeightMap, const CPUTexture2D &layerWeightsMap, DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		vertices->Reserve((resolution + 1) * (resolution + 1) * 5);
		indices->Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				const float textureCoordinateX{ static_cast<float>(i) / static_cast<float>(resolution) };
				const float textureCoordinateY{ static_cast<float>(j) / static_cast<float>(resolution) };

				const Vector4 &normalHeight{ normalHeightMap.At(textureCoordinateX, textureCoordinateY) };
				const Vector4 &layerWeight{ layerWeightsMap.At(textureCoordinateX, textureCoordinateY) };

				TerrainVertex vertex;

				vertex._PositionX = worldPosition._X + ((-1.0f + (2.0f * textureCoordinateX)) * (properties._PatchSize * 0.5f));
				vertex._PositionZ = worldPosition._Z + ((-1.0f + (2.0f * textureCoordinateY)) * (properties._PatchSize * 0.5f));
				properties._HeightGenerationFunction(properties, Vector3(vertex._PositionX, 0.0f, vertex._PositionZ), &vertex._PositionY);

				vertex._NormalX = normalHeight._X;
				vertex._NormalY = normalHeight._Y;
				vertex._NormalZ = normalHeight._Z;

				vertex._LayerWeightX = layerWeight._X;
				vertex._LayerWeightY = layerWeight._Y;
				vertex._LayerWeightZ = layerWeight._Z;
				vertex._LayerWeightW = layerWeight._W;

				vertex._TextureCoordinateX = textureCoordinateX * properties._TerrainTextureTilingFactor;
				vertex._TextureCoordinateY = textureCoordinateY * properties._TerrainTextureTilingFactor;

				vertices->EmplaceFast(vertex);

				if (i != resolution && j != resolution)
				{
					indices->EmplaceFast((i * (resolution + 1)) + j);
					indices->EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices->EmplaceFast((i * (resolution + 1)) + j + 1);

					indices->EmplaceFast((i * (resolution + 1)) + j + 1);
					indices->EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices->EmplaceFast(((i + 1) * (resolution + 1)) + j + 1);
				}
			}
		}
	}

}