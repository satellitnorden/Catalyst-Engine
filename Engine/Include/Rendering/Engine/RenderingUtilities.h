#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/Vector4.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/TerrainUniformData.h>
#include <Rendering/Engine/VegetationTransformation.h>

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

		const CameraComponent *const RESTRICT cameraComponent{ ComponentManager::GetCameraCameraComponents() };
		const DirectionalLightComponent *const RESTRICT directionalLightComponent{ ComponentManager::GetDirectionalLightDirectionalLightComponents() };

		const Vector3 cameraPosition{ cameraComponent->_Position };
		const Vector3 directionalLightForwardVector{ Vector3(0.0f, 0.0f, -1.0f).Rotated(directionalLightComponent->_Rotation) };
		const Vector3 directionalLightUpVector{ Vector3(0.0f, 1.0f, 0.0f).Rotated(directionalLightComponent->_Rotation) };
		const Vector3 directionalLightPosition{ cameraPosition + (directionalLightForwardVector * -1.0f) * (RenderingConstants::SHADOW_VIEW_DISTANCE * 0.5f) };

		const Matrix4 directionalLightDirection{ Matrix4::LookAt(directionalLightPosition, directionalLightPosition + directionalLightForwardVector, directionalLightUpVector) };
	
		const Matrix4 directionalLightViewMatrix{ directionalLightProjectionMatrix * directionalLightDirection };

		const Vector4 test{ directionalLightViewMatrix * Vector4(cameraPosition , 1.0f) };

		return directionalLightProjectionMatrix * directionalLightDirection;
	}

	/*
	*	Calculates an axis-aligned bounding box for a terrain.
	*/
	static void CalculateTerrainAxisAlignedBoundingBox(const CPUTexture2D *const RESTRICT properties, const float extent, const Vector3 *const RESTRICT _WorldPosition, const float height, AxisAlignedBoundingBox *const RESTRICT box) NOEXCEPT
	{
		float lowest{ FLOAT_MAXIMUM };
		float highest{ -FLOAT_MAXIMUM };

		for (const Vector4 &property : *properties)
		{
			lowest = CatalystBaseMath::Minimum(lowest, property._W);
			highest = CatalystBaseMath::Maximum(highest, property._W);
		}

		const float halfExtent{ extent * 0.5f };

		box->_Minimum = *_WorldPosition + Vector3(-halfExtent, lowest * height, -halfExtent);
		box->_Maximum = *_WorldPosition + Vector3(halfExtent, highest * height, halfExtent);
	}

	/*
	*	Calculates the vegetation grid. Outputs a new container for the sorted transformations.
	*/
	static void CalculateVegetationGrid(const float cutoffDistance, const DynamicArray<VegetationTransformation> &transformations, VegetationComponent *const RESTRICT renderComponent, VegetationCullingComponent *const RESTRICT cullingComponent, DynamicArray<VegetationTransformation> &sortedTransformations) NOEXCEPT
	{
		//Set the squared cutoff distance.
		cullingComponent->_CutoffDistance = cutoffDistance * 2.0f;

		//Calculate the bounding box of all transformations.
		Vector2 gridMinimum{ FLOAT_MAXIMUM, FLOAT_MAXIMUM };
		Vector2 gridMaximum{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

		for (const VegetationTransformation &transformation : transformations)
		{
			gridMinimum._X = CatalystBaseMath::Minimum<float>(gridMinimum._X, transformation._Position._X);
			gridMinimum._Y = CatalystBaseMath::Minimum<float>(gridMinimum._Y, transformation._Position._Z);
			gridMaximum._X = CatalystBaseMath::Maximum<float>(gridMaximum._X, transformation._Position._X);
			gridMaximum._Y = CatalystBaseMath::Maximum<float>(gridMaximum._Y, transformation._Position._Z);
		}

		//Now that the bounding box extent is known, calculate the number of rows/columns for the grid.
		const float xExtent{ gridMaximum._X - gridMinimum._X };
		const float yExtent{ gridMaximum._Y - gridMinimum._Y };

		const float halfXExtent{ xExtent * 0.5f };
		const float halfYExtent{ yExtent * 0.5f };

		const uint64 rows{ CatalystBaseMath::Round<uint64>(xExtent / cutoffDistance) };
		const uint64 columns{ CatalystBaseMath::Round<uint64>(yExtent / cutoffDistance) };

		//Resize all containers accordingly.
		const uint64 containerSize{ rows * columns };
		renderComponent->_ShouldDrawGridCell.UpsizeFast(containerSize);
		renderComponent->_InstanceCounts.UpsizeFast(containerSize);
		renderComponent->_TransformationOffsets.UpsizeFast(containerSize);
		cullingComponent->_GridCellCenterLocations.UpsizeFast(containerSize);

		//Calculate all cell center positions.
		const float rowSize{ xExtent / static_cast<float>(rows) };
		const float columnSize{ yExtent / static_cast<float>(columns) };

		const float halfRowSize{ rowSize * 0.5f };
		const float halfColumnSize{ columnSize * 0.5f };

		for (uint64 i = 0; i < rows; ++i)
		{
			for (uint64 j = 0; j < columns; ++j)
			{
				cullingComponent->_GridCellCenterLocations[(i * columns) + j] = Vector2(	gridMinimum._X + halfRowSize + (rowSize * i),
																						gridMinimum._Y + halfRowSize + (columnSize * j));
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
			uint64 rowIndex{ CatalystBaseMath::Floor<uint64>(((transformation._Position._X + halfXExtent) * inverseXExtent) * static_cast<float>(rows)) };
			uint64 columnIndex{ CatalystBaseMath::Floor<uint64>(((transformation._Position._Z + halfYExtent) * inverseYExtent) * static_cast<float>(columns)) };

			rowIndex = CatalystBaseMath::Minimum<uint64>(rowIndex, rows - 1);
			columnIndex = CatalystBaseMath::Minimum<uint64>(columnIndex, columns - 1);

			temporaryTransformations[rowIndex][columnIndex].EmplaceSlow(transformation);
		}

		sortedTransformations.UpsizeFast(transformations.Size());

		uint64 offset{ 0 };

		for (uint64 i = 0; i < rows; ++i)
		{
			for (uint64 j = 0; j < columns; ++j)
			{
				renderComponent->_InstanceCounts[(i * columns) + j] = static_cast<uint32>(temporaryTransformations[i][j].Size());
				renderComponent->_TransformationOffsets[(i * columns) + j] = offset;

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

				vertices.EmplaceFast(-1.0f + (2.0f * xTextureCoordinate));
				vertices.EmplaceFast(0.0f);
				vertices.EmplaceFast(-1.0f + (2.0f * yTextureCoordinate));
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
			highestX = CatalystBaseMath::Maximum(highestX, corners[i]._X);
			lowestX = CatalystBaseMath::Minimum(lowestX, corners[i]._X);
			highestY = CatalystBaseMath::Maximum(highestY, corners[i]._Y);
			lowestY = CatalystBaseMath::Minimum(lowestY, corners[i]._Y);
			highestZ = CatalystBaseMath::Maximum(highestZ, corners[i]._Z);
			lowestZ = CatalystBaseMath::Minimum(lowestZ, corners[i]._Z);
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
	static bool IsInViewFrustum(const Matrix4 &viewMatrix, const AxisAlignedBoundingBox &axisAlignedBoundingBox) NOEXCEPT
	{
		StaticArray<Vector4, 8> corners;

		corners[0] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);
		corners[1] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);
		corners[2] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);
		corners[3] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Minimum._Z, 1.0f);

		corners[4] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);
		corners[5] = Vector4(axisAlignedBoundingBox._Minimum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);
		corners[6] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Maximum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);
		corners[7] = Vector4(axisAlignedBoundingBox._Maximum._X, axisAlignedBoundingBox._Minimum._Y, axisAlignedBoundingBox._Maximum._Z, 1.0f);

		for (uint8 i = 0; i < 8; ++i)
		{
			corners[i] = viewMatrix * corners[i];

			const float inverseW{ 1.0f / corners[i]._W };

			corners[i]._X *= inverseW;
			corners[i]._Y *= inverseW;
			corners[i]._Z *= inverseW;
		}

		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = CatalystBaseMath::Maximum(highestX, corners[i]._X);
			lowestX = CatalystBaseMath::Minimum(lowestX, corners[i]._X);
			highestY = CatalystBaseMath::Maximum(highestY, corners[i]._Y);
			lowestY = CatalystBaseMath::Minimum(lowestY, corners[i]._Y);
			highestZ = CatalystBaseMath::Maximum(highestZ, corners[i]._Z);
			lowestZ = CatalystBaseMath::Minimum(lowestZ, corners[i]._Z);
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