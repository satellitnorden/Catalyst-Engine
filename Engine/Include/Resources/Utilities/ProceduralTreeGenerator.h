#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

class ProceduralTreeGenerator final
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

	};

	/*
	*	Output class definition.
	*/
	class Output final
	{

	public:

		//The vertices.
		DynamicArray<DynamicArray<Vertex>> _Vertices;

		//The indices.
		DynamicArray<DynamicArray<uint32>> _Indices;

		//The collision vertices.
		DynamicArray<Vertex> _CollisionVertices;

		//The collision indices.
		DynamicArray<uint32> _CollisionIndices;

	};

	/*
	*	Generates a tree with the given parameters.
	*	Outputs vertices and indices.
	*/
	static void GenerateTree(const Parameters &parameters, Output *const RESTRICT output) NOEXCEPT;
};