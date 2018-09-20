#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Resources.
#include <Resources/PhysicalModeldata.h>

namespace CommonPhysicalModelData
{

	/*
	*	Generates cube physical model data->
	*/
	static void GetCubePhysicalModelData(PhysicalModelData *const RESTRICT data) NOEXCEPT
	{
		data->_Extent = 1.0f;

		data->_Vertices.Reserve(24);

		//Left side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, -0.5f, -0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.5f, -0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.5f, 0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, -0.5f, 0.5f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Front side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, -0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, -0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Right side.
		data->_Vertices.EmplaceFast(Vector3(0.5f, -0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.5f, -0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, -0.5f, -0.5f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Back side.
		data->_Vertices.EmplaceFast(Vector3(0.5f, -0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f));

		//Down side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, -0.5f, 0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, -0.5f, 0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, -0.5f, -0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f));

		//Up side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.5f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.5f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.5f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f));

		data->_Indices.Reserve(36);

		//Left side.
		data->_Indices.EmplaceFast(0);
		data->_Indices.EmplaceFast(2);
		data->_Indices.EmplaceFast(1);
		data->_Indices.EmplaceFast(0);
		data->_Indices.EmplaceFast(3);
		data->_Indices.EmplaceFast(2);

		//Front side.
		data->_Indices.EmplaceFast(4);
		data->_Indices.EmplaceFast(6);
		data->_Indices.EmplaceFast(5);
		data->_Indices.EmplaceFast(4);
		data->_Indices.EmplaceFast(7);
		data->_Indices.EmplaceFast(6);

		//Right side.
		data->_Indices.EmplaceFast(8);
		data->_Indices.EmplaceFast(10);
		data->_Indices.EmplaceFast(9);
		data->_Indices.EmplaceFast(8);
		data->_Indices.EmplaceFast(11);
		data->_Indices.EmplaceFast(10);

		//Back side.
		data->_Indices.EmplaceFast(12);
		data->_Indices.EmplaceFast(14);
		data->_Indices.EmplaceFast(13);
		data->_Indices.EmplaceFast(12);
		data->_Indices.EmplaceFast(15);
		data->_Indices.EmplaceFast(14);

		//Down side.
		data->_Indices.EmplaceFast(16);
		data->_Indices.EmplaceFast(18);
		data->_Indices.EmplaceFast(17);
		data->_Indices.EmplaceFast(16);
		data->_Indices.EmplaceFast(19);
		data->_Indices.EmplaceFast(18);

		//Up side.
		data->_Indices.EmplaceFast(20);
		data->_Indices.EmplaceFast(22);
		data->_Indices.EmplaceFast(21);
		data->_Indices.EmplaceFast(20);
		data->_Indices.EmplaceFast(23);
		data->_Indices.EmplaceFast(22);
	}

	/*
	*	Generates octahedron physical model data->
	*/
	static void GetOctahedronPhysicalModelData(PhysicalModelData *const RESTRICT data) NOEXCEPT
	{
		data->_Extent = 1.0f;

		data->_Vertices.Reserve(24);

		//Bottom left side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, -0.5f), Vector3(-0.707'106'769f, -0.707'106'769f, 0.0f), Vector3(0.707'106'769f, -0.707'106'769f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, 0.5f), Vector3(-0.707'106'769f, -0.707'106'769f, 0.0f), Vector3(0.707'106'769f, -0.707'106'769f, 0.0f), Vector2(1.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, -0.5f, 0.0f), Vector3(-0.707'106'769f, -0.707'106'769f, 0.0f), Vector3(0.707'106'769f, -0.707'106'769f, 0.0f), Vector2(0.5f, 1.0f));

		//Bottom front side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, 0.5f), Vector3(0.0f, -0.707'106'769f, 0.707'106'769f), Vector3(0.0f, -0.707'106'769f, -0.707'106'769f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, 0.5f), Vector3(0.0f, -0.707'106'769f, 0.707'106'769f), Vector3(0.0f, -0.707'106'769f, -0.707'106'769f), Vector2(1.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, -0.5f, 0.0f), Vector3(0.0f, -0.707'106'769f, 0.707'106'769f), Vector3(0.0f, -0.707'106'769f, -0.707'106'769f), Vector2(0.5f, 1.0f));

		//Bottom right side.
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, 0.5f), Vector3(0.707'106'769f, -0.707'106'769f, 0.0f), Vector3(-0.707'106'769f, -0.707'106'769f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, -0.5f), Vector3(0.707'106'769f, -0.707'106'769f, 0.0f), Vector3(-0.707'106'769f, -0.707'106'769f, 0.0f), Vector2(1.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, -0.5f, 0.0f), Vector3(0.707'106'769f, -0.707'106'769f, 0.0f), Vector3(-0.707'106'769f, -0.707'106'769f, 0.0f), Vector2(0.5f, 1.0f));

		//Bottom back side.
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, -0.5f), Vector3(0.0f, -0.707'106'769f, -0.707'106'769f), Vector3(0.0f, -0.707'106'769f, 0.707'106'769f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.0f, -0.707'106'769f, -0.707'106'769f), Vector3(0.0f, -0.707'106'769f, 0.707'106'769f), Vector2(1.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, -0.5f, 0.0f), Vector3(0.0f, -0.707'106'769f, -0.707'106'769f), Vector3(0.0f, -0.707'106'769f, 0.707'106'769f), Vector2(0.5f, 1.0f));

		//Top left side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, -0.5f), Vector3(-0.707'106'769f, 0.707'106'769f, 0.0f), Vector3(0.707'106'769f, 0.707'106'769f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, 0.5f, 0.0f), Vector3(-0.707'106'769f, 0.707'106'769f, 0.0f), Vector3(0.707'106'769f, 0.707'106'769f, 0.0f), Vector2(0.5f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, 0.5f), Vector3(-0.707'106'769f, 0.707'106'769f, 0.0f), Vector3(0.707'106'769f, 0.707'106'769f, 0.0f), Vector2(1.0f, 0.0f));

		//Top front side.
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, 0.5f), Vector3(0.0f, 0.707'106'769f, 0.707'106'769f), Vector3(0.0f, 0.707'106'769f, -0.707'106'769f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, 0.5f, 0.0f), Vector3(0.0f, 0.707'106'769f, 0.707'106'769f), Vector3(0.0f, 0.707'106'769f, -0.707'106'769f), Vector2(0.5f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, 0.5f), Vector3(0.0f, 0.707'106'769f, 0.707'106'769f), Vector3(0.0f, 0.707'106'769f, -0.707'106'769f), Vector2(1.0f, 0.0f));

		//Top right side.
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, 0.5f), Vector3(0.707'106'769f, 0.707'106'769f, 0.0f), Vector3(-0.707'106'769f, 0.707'106'769f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, 0.5f, 0.0f), Vector3(0.707'106'769f, 0.707'106'769f, 0.0f), Vector3(-0.707'106'769f, 0.707'106'769f, 0.0f), Vector2(0.5f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, -0.5f), Vector3(0.707'106'769f, 0.707'106'769f, 0.0f), Vector3(-0.707'106'769f, 0.707'106'769f, 0.0f), Vector2(1.0f, 0.0f));

		//Top back side.
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, -0.5f), Vector3(0.0f, 0.707'106'769f, -0.707'106'769f), Vector3(0.0f, 0.707'106'769f, 0.707'106'769f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(0.0f, 0.5f, 0.0f), Vector3(0.0f, 0.707'106'769f, -0.707'106'769f), Vector3(0.0f, 0.707'106'769f, 0.707'106'769f), Vector2(0.5f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.0f, 0.707'106'769f, -0.707'106'769f), Vector3(0.0f, 0.707'106'769f, 0.707'106'769f), Vector2(1.0f, 0.0f));

		data->_Indices.Reserve(24);

		data->_Indices.EmplaceFast(0);
		data->_Indices.EmplaceFast(2);
		data->_Indices.EmplaceFast(1);

		data->_Indices.EmplaceFast(3);
		data->_Indices.EmplaceFast(5);
		data->_Indices.EmplaceFast(4);

		data->_Indices.EmplaceFast(6);
		data->_Indices.EmplaceFast(8);
		data->_Indices.EmplaceFast(7);

		data->_Indices.EmplaceFast(9);
		data->_Indices.EmplaceFast(11);
		data->_Indices.EmplaceFast(10);

		data->_Indices.EmplaceFast(12);
		data->_Indices.EmplaceFast(14);
		data->_Indices.EmplaceFast(13);

		data->_Indices.EmplaceFast(15);
		data->_Indices.EmplaceFast(17);
		data->_Indices.EmplaceFast(16);

		data->_Indices.EmplaceFast(18);
		data->_Indices.EmplaceFast(20);
		data->_Indices.EmplaceFast(19);

		data->_Indices.EmplaceFast(21);
		data->_Indices.EmplaceFast(23);
		data->_Indices.EmplaceFast(22);
	}

	/*
	*	Generates plane physical model data->
	*/
	static void GetPlanePhysicalModelData(PhysicalModelData *const RESTRICT data) NOEXCEPT
	{
		data->_Extent = 1.0f;

		data->_Vertices.Reserve(4);
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f));
		data->_Vertices.EmplaceFast(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, -0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f));
		data->_Vertices.EmplaceFast(Vector3(0.5f, 0.0f, 0.5f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f));

		data->_Indices.Reserve(6);
		data->_Indices.EmplaceFast(0);
		data->_Indices.EmplaceFast(2);
		data->_Indices.EmplaceFast(1);
		data->_Indices.EmplaceFast(0);
		data->_Indices.EmplaceFast(3);
		data->_Indices.EmplaceFast(2);
	}

}