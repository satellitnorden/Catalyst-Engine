#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

/*
*	Class representing the raw data read from a model file (for example .fbx files).
*/
class ModelFile final
{

public:

	/*
	*	Mesh class definition.
	*/
	class Mesh final
	{

	public:

		//The vertices.
		DynamicArray<Vertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

	};

	//The meshes.
	DynamicArray<Mesh> _Meshes;

};