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

	//Enumeration covering all creators.
	enum class Creator : uint8
	{
		BLENDER,

		UNKNOWN
	};

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

	//The creator.
	Creator _Creator{ Creator::UNKNOWN };

	//The meshes.
	DynamicArray<Mesh> _Meshes;

	/*
	*	Returns if this model file is valid.
	*/
	NO_DISCARD bool IsValid() const NOEXCEPT;

	/*
	*	Post processes this model file.
	*/
	void PostProcess() NOEXCEPT;

};