#pragma once

//Core.
#include <Core/EngineCore.h>

//Resources.
#include <Resources/PhysicalModelData.h>

namespace CommonPhysicalModelData
{

	/*
	*	Generates cube model data.
	*/
	static void GetCubePhysicalModelData(PhysicalModelData &data) NOEXCEPT
	{
		data.extent = 1.0f;

		data.vertices.Reserve(24);

		//Left side.
		data.vertices.EmplaceFast(Vector3(-0.5f, -0.5f, -0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.5f, -0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.5f, 0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, -0.5f, 0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Front side.
		data.vertices.EmplaceFast(Vector3(-0.5f, -0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, -0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Right side.
		data.vertices.EmplaceFast(Vector3(0.5f, -0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.5f, -0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, -0.5f, -0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Back side.
		data.vertices.EmplaceFast(Vector3(0.5f, -0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Down side.
		data.vertices.EmplaceFast(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, -0.5f, 0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, -0.5f, 0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, -0.5f, -0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f));

		//Up side.
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.5f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.5f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.5f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f));

		data.indices.Reserve(36);

		//Left side.
		data.indices.EmplaceFast(0);
		data.indices.EmplaceFast(2);
		data.indices.EmplaceFast(1);
		data.indices.EmplaceFast(0);
		data.indices.EmplaceFast(3);
		data.indices.EmplaceFast(2);

		//Front side.
		data.indices.EmplaceFast(4);
		data.indices.EmplaceFast(6);
		data.indices.EmplaceFast(5);
		data.indices.EmplaceFast(4);
		data.indices.EmplaceFast(7);
		data.indices.EmplaceFast(6);

		//Right side.
		data.indices.EmplaceFast(8);
		data.indices.EmplaceFast(10);
		data.indices.EmplaceFast(9);
		data.indices.EmplaceFast(8);
		data.indices.EmplaceFast(11);
		data.indices.EmplaceFast(10);

		//Back side.
		data.indices.EmplaceFast(12);
		data.indices.EmplaceFast(14);
		data.indices.EmplaceFast(13);
		data.indices.EmplaceFast(12);
		data.indices.EmplaceFast(15);
		data.indices.EmplaceFast(14);

		//Down side.
		data.indices.EmplaceFast(16);
		data.indices.EmplaceFast(18);
		data.indices.EmplaceFast(17);
		data.indices.EmplaceFast(16);
		data.indices.EmplaceFast(19);
		data.indices.EmplaceFast(18);

		//Up side.
		data.indices.EmplaceFast(20);
		data.indices.EmplaceFast(22);
		data.indices.EmplaceFast(21);
		data.indices.EmplaceFast(20);
		data.indices.EmplaceFast(23);
		data.indices.EmplaceFast(22);
	}

	/*
	*	Generates plane model data.
	*/
	static void GetPlanePhysicalModelData(PhysicalModelData &data) NOEXCEPT
	{
		constexpr float textureCoordinateMultiplier{ 10.0f };

		data.extent = 0.5f;

		data.vertices.Reserve(4);
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.0f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f));
		data.vertices.EmplaceFast(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f * textureCoordinateMultiplier));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.0f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f * textureCoordinateMultiplier, 1.0f * textureCoordinateMultiplier));
		data.vertices.EmplaceFast(Vector3(0.5f, 0.0f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f * textureCoordinateMultiplier, 0.0f));

		data.indices.Reserve(6);
		data.indices.EmplaceFast(0);
		data.indices.EmplaceFast(2);
		data.indices.EmplaceFast(1);
		data.indices.EmplaceFast(0);
		data.indices.EmplaceFast(3);
		data.indices.EmplaceFast(2);
	}

}