#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Vertex.h>

class Mesh final
{

public:

	/*
	*	Mesh level of detail class definition.
	*/
	class MeshLevelOfDetail final
	{

	public:

		//The vertices.
		DynamicArray<Vertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

		//The vertex buffer.
		BufferHandle _VertexBuffer;

		//The index buffer.
		BufferHandle _IndexBuffer;

		//The index count.
		uint32 _IndexCount;

	};

	//All the mesh level of details.
	DynamicArray<MeshLevelOfDetail> _MeshLevelOfDetails;

};