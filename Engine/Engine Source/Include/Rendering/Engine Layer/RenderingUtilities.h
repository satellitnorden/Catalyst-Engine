#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/CatalystMath.h>
#include <Math/Vector4.h>

//Rendering.
#include <Rendering/Engine Layer/PhysicalVertex.h>
#include <Rendering/Engine Layer/RenderingCore.h>
#include <Rendering/Engine Layer/TerrainUniformData.h>
#include <Rendering/Engine Layer/VegetationTransformation.h>
#include <Rendering/Engine Layer/WaterUniformData.h>

//Resources.
#include <Resources/PhysicalModelData.h>

namespace RenderingUtilities
{

	/*
	*	Calculates the directional light view matrix.
	*/
	static Matrix4 CalculateDirectionalLightViewMatrix() NOEXCEPT
	{
		static Matrix4 directionalLightProjectionMatrix{ Matrix4::Ortographic(-RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, -RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f, 0.0f, RenderingConstants::SHADOW_VIEW_DISTANCE) };

		const CameraComponent *const RESTRICT cameraComponent{ ComponentManager::GetCameraComponents() };
		const DirectionalLightComponent *const RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightComponents() };

		const Vector3 cameraPosition{ cameraComponent->position };
		const Vector3 directionalLightForwardVector{ Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->rotation) };
		const Vector3 directionalLightUpVector{ Vector3(0.0f, 1.0f, 0.0f).Rotated(directionalLightComponent->rotation) };
		const Vector3 directionalLightPosition{ cameraPosition + (directionalLightForwardVector * -1.0f) * (RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f) };

		const Matrix4 directionalLightDirection{ Matrix4::LookAt(directionalLightPosition, directionalLightPosition + directionalLightForwardVector, directionalLightUpVector) };
	
		const Matrix4 directionalLightViewMatrix{ directionalLightProjectionMatrix * directionalLightDirection };

		const Vector4 test{ directionalLightViewMatrix * Vector4(cameraPosition , 1.0f) };

		return directionalLightProjectionMatrix * directionalLightDirection;
	}

	/*
	*	Calculates the vegetation grid. Outputs a new container for the sorted transformations.
	*/
	static void CalculateVegetationGrid(const float cutoffDistance, const DynamicArray<VegetationTransformation> &transformations, VegetationComponent *const RESTRICT renderComponent, VegetationCullingComponent *const RESTRICT cullingComponent, DynamicArray<VegetationTransformation> &sortedTransformations) NOEXCEPT
	{
		//Set the squared cutoff distance.
		cullingComponent->cutoffDistance = cutoffDistance * 2.0f;

		//Calculate the bounding box of all transformations.
		Vector2 gridMinimum{ FLOAT_MAXIMUM, FLOAT_MAXIMUM };
		Vector2 gridMaximum{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

		for (const VegetationTransformation &transformation : transformations)
		{
			gridMinimum.X = CatalystMath::Minimum<float>(gridMinimum.X, transformation.position.X);
			gridMinimum.Y = CatalystMath::Minimum<float>(gridMinimum.Y, transformation.position.Z);
			gridMaximum.X = CatalystMath::Maximum<float>(gridMaximum.X, transformation.position.X);
			gridMaximum.Y = CatalystMath::Maximum<float>(gridMaximum.Y, transformation.position.Z);
		}

		//Now that the bounding box extent is known, calculate the number of rows/columns for the grid.
		const float xExtent{ gridMaximum.X - gridMinimum.X };
		const float yExtent{ gridMaximum.Y - gridMinimum.Y };

		const float halfXExtent{ xExtent * 0.5f };
		const float halfYExtent{ yExtent * 0.5f };

		const uint64 rows{ CatalystMath::Round<uint64>(xExtent / cutoffDistance) };
		const uint64 columns{ CatalystMath::Round<uint64>(yExtent / cutoffDistance) };

		//Resize all containers accordingly.
		const uint64 containerSize{ rows * columns };
		renderComponent->shouldDrawGridCell.UpsizeFast(containerSize);
		renderComponent->instanceCounts.UpsizeFast(containerSize);
		renderComponent->transformationOffsets.UpsizeFast(containerSize);
		cullingComponent->gridCellCenterLocations.UpsizeFast(containerSize);

		//Calculate all cell center positions.
		const float rowSize{ xExtent / static_cast<float>(rows) };
		const float columnSize{ yExtent / static_cast<float>(columns) };

		const float halfRowSize{ rowSize * 0.5f };
		const float halfColumnSize{ columnSize * 0.5f };

		for (uint64 i = 0; i < rows; ++i)
		{
			for (uint64 j = 0; j < columns; ++j)
			{
				cullingComponent->gridCellCenterLocations[(i * columns) + j] = Vector2(	gridMinimum.X + halfRowSize + (rowSize * i),
																						gridMinimum.Y + halfRowSize + (columnSize * j));
			}
		}

		//Fill the new, sorted transformations array.
		DynamicArray<DynamicArray<DynamicArray<VegetationTransformation>>> temporaryTransformations;

		temporaryTransformations.UpsizeSlow(rows);

		const uint64 approximatedTransformationsPerGridCell{ transformations.Size() / containerSize };

		for (DynamicArray<DynamicArray<VegetationTransformation>> & temporaryTransformation : temporaryTransformations)
		{
			temporaryTransformation.UpsizeSlow(columns);
		}

		const float inverseXExtent{ 1.0f / xExtent };
		const float inverseYExtent{ 1.0f / yExtent };

		for (const VegetationTransformation &transformation : transformations)
		{
			uint64 rowIndex{ CatalystMath::Floor<uint64>(((transformation.position.X + halfXExtent) * inverseXExtent) * static_cast<float>(rows)) };
			uint64 columnIndex{ CatalystMath::Floor<uint64>(((transformation.position.Z + halfYExtent) * inverseYExtent) * static_cast<float>(columns)) };

			rowIndex = CatalystMath::Minimum<uint64>(rowIndex, rows - 1);
			columnIndex = CatalystMath::Minimum<uint64>(columnIndex, columns - 1);

			temporaryTransformations[rowIndex][columnIndex].EmplaceSlow(transformation);
		}

		sortedTransformations.UpsizeFast(transformations.Size());

		uint64 offset{ 0 };

		for (uint64 i = 0; i < rows; ++i)
		{
			for (uint64 j = 0; j < columns; ++j)
			{
				renderComponent->instanceCounts[(i * columns) + j] = static_cast<uint32>(temporaryTransformations[i][j].Size());
				renderComponent->transformationOffsets[(i * columns) + j] = offset;

				const uint64 dataSize{ sizeof(VegetationTransformation) * temporaryTransformations[i][j].Size() };

				MemoryUtilities::CopyMemory(reinterpret_cast<byte *const RESTRICT>(sortedTransformations.Data()) + offset, temporaryTransformations[i][j].Data(), dataSize);

				offset += dataSize;
			}
		}
	}

	/*
	*	Given terrain properties, terrain uniform data and a resolution, generate terrain plane vertices and indices.
	*/
	static void GenerateTerrainPlane(const uint32 resolution, DynamicArray<float> &vertices, DynamicArray<uint32> &indices) NOEXCEPT
	{
		vertices.Reserve((resolution + 1) * (resolution + 1) * 5);
		indices.Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				const float xTextureCoordinate{ static_cast<float>(i) / static_cast<float>(resolution) };
				const float yTextureCoordinate{ static_cast<float>(j) / static_cast<float>(resolution) };

				vertices.EmplaceFast(-1.0f + (2.0f * static_cast<float>(i) / static_cast<float>(resolution)));
				vertices.EmplaceFast(0.0f);
				vertices.EmplaceFast(-1.0f + (2.0f * static_cast<float>(j) / static_cast<float>(resolution)));
				vertices.EmplaceFast(xTextureCoordinate);
				vertices.EmplaceFast(yTextureCoordinate);

				if (i != resolution && j != resolution)
				{
					indices.EmplaceFast((i * (resolution + 1)) + j);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices.EmplaceFast((i * (resolution + 1)) + j + 1);

					indices.EmplaceFast((i * (resolution + 1)) + j + 1);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j + 1);
				}
			}
		}
	}

	/*
	*	Given a resolution and the water uniform data, generate water vertices and indices.
	*/
	static void GenerateWaterPlane(const uint32 resolution, const WaterUniformData waterUniformData, DynamicArray<float> &vertices, DynamicArray<uint32> &indices) NOEXCEPT
	{
		vertices.Reserve((resolution + 1) * (resolution + 1) * 5);
		indices.Reserve(resolution * resolution * 6);

		for (uint32 i = 0; i <= resolution; ++i)
		{
			for (uint32 j = 0; j <= resolution; ++j)
			{
				const float xTextureCoordinate{ static_cast<float>(i) / static_cast<float>(resolution) };
				const float yTextureCoordinate{ static_cast<float>(j) / static_cast<float>(resolution) };

				vertices.EmplaceFast((-1.0f + (2.0f * static_cast<float>(i) / static_cast<float>(resolution))) * (waterUniformData.waterSize * 0.5f) + waterUniformData.waterWorldPosition.X);
				vertices.EmplaceFast(0.0f);
				vertices.EmplaceFast((-1.0f + (2.0f * static_cast<float>(j) / static_cast<float>(resolution))) * (waterUniformData.waterSize * 0.5f) + waterUniformData.waterWorldPosition.Z);
				vertices.EmplaceFast(xTextureCoordinate);
				vertices.EmplaceFast(yTextureCoordinate);

				if (i != resolution && j != resolution)
				{
					indices.EmplaceFast((i * (resolution + 1)) + j);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices.EmplaceFast((i * (resolution + 1)) + j + 1);

					indices.EmplaceFast((i * (resolution + 1)) + j + 1);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j);
					indices.EmplaceFast(((i + 1) * (resolution + 1)) + j + 1);
				}
			}
		}
	}

	/*
	*	Generates plane model data.
	*/
	static void GetPlanePhysicalModelData(PhysicalModelData &data) NOEXCEPT
	{
		data.extent = 0.5f;

		data.vertices.Reserve(4);

	}

	/*
	*	Given 8 corners of a cube, determine if it is within the view frustum.
	*/
	static bool IsCubeWithinViewFrustum(const StaticArray<Vector4, 8> &corners) NOEXCEPT
	{
		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = CatalystMath::Maximum(highestX, corners[i].X);
			lowestX = CatalystMath::Minimum(lowestX, corners[i].X);
			highestY = CatalystMath::Maximum(highestY, corners[i].Y);
			lowestY = CatalystMath::Minimum(lowestY, corners[i].Y);
			highestZ = CatalystMath::Maximum(highestZ, corners[i].Z);
			lowestZ = CatalystMath::Minimum(lowestZ, corners[i].Z);
		}

		if (((highestX > 1.0f && lowestX > 1.0f) || (highestX < -1.0f && lowestX < -1.0f))
			||
			((highestY > 1.0f && lowestY > 1.0f) || (highestY < -1.0f && lowestY < -1.0f))
			||
			((highestZ > 1.0f && lowestZ > 1.0f) || (highestZ < 0.0f && lowestZ < 0.0f)))
		{
			return false;
		}

		else
		{
			return true;
		}
	}

	/*
	*	Given a view matrix and an axis-aligned bounding box, returns if the box is in the view frustum.
	*/
	static bool IsInViewFrustum(const Matrix4 &viewMatrix, const Vector3 &worldPosition, const AxisAlignedBoundingBox &axisAlignedBoundingBox) NOEXCEPT
	{
		StaticArray<Vector4, 8> corners;

		corners[0] = Vector4(axisAlignedBoundingBox.minimum.X, axisAlignedBoundingBox.minimum.Y, axisAlignedBoundingBox.minimum.Z, 1.0f);
		corners[1] = Vector4(axisAlignedBoundingBox.minimum.X, axisAlignedBoundingBox.maximum.Y, axisAlignedBoundingBox.minimum.Z, 1.0f);
		corners[2] = Vector4(axisAlignedBoundingBox.maximum.X, axisAlignedBoundingBox.maximum.Y, axisAlignedBoundingBox.minimum.Z, 1.0f);
		corners[3] = Vector4(axisAlignedBoundingBox.maximum.X, axisAlignedBoundingBox.minimum.Y, axisAlignedBoundingBox.minimum.Z, 1.0f);

		corners[4] = Vector4(axisAlignedBoundingBox.minimum.X, axisAlignedBoundingBox.minimum.Y, axisAlignedBoundingBox.maximum.Z, 1.0f);
		corners[5] = Vector4(axisAlignedBoundingBox.minimum.X, axisAlignedBoundingBox.maximum.Y, axisAlignedBoundingBox.maximum.Z, 1.0f);
		corners[6] = Vector4(axisAlignedBoundingBox.maximum.X, axisAlignedBoundingBox.maximum.Y, axisAlignedBoundingBox.maximum.Z, 1.0f);
		corners[7] = Vector4(axisAlignedBoundingBox.maximum.X, axisAlignedBoundingBox.minimum.Y, axisAlignedBoundingBox.maximum.Z, 1.0f);

		for (uint8 i = 0; i < 8; ++i)
		{
			corners[i] += Vector4(worldPosition.X, worldPosition.Y, worldPosition.Z, 0.0f);

			corners[i] = viewMatrix * corners[i];

			const float inverseW{ 1.0f / corners[i].W };

			corners[i].X *= inverseW;
			corners[i].Y *= inverseW;
			corners[i].Z *= inverseW;
		}

		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = CatalystMath::Maximum(highestX, corners[i].X);
			lowestX = CatalystMath::Minimum(lowestX, corners[i].X);
			highestY = CatalystMath::Maximum(highestY, corners[i].Y);
			lowestY = CatalystMath::Minimum(lowestY, corners[i].Y);
			highestZ = CatalystMath::Maximum(highestZ, corners[i].Z);
			lowestZ = CatalystMath::Minimum(lowestZ, corners[i].Z);
		}

		if (((highestX > 1.0f && lowestX > 1.0f) || (highestX < -1.0f && lowestX < -1.0f))
			||
			((highestY > 1.0f && lowestY > 1.0f) || (highestY < -1.0f && lowestY < -1.0f))
			||
			((highestZ > 1.0f && lowestZ > 1.0f) || (highestZ < 0.0f && lowestZ < 0.0f)))
		{
			return false;
		}

		else
		{
			return true;
		}
	}
}