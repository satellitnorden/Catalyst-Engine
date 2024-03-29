#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Animation.
#include <Animation/AnimatedVertex.h>
#include <Animation/Skeleton.h>

/*
*	Class representing the raw data read from an animated model file (for example .fbx files).
*/
class AnimatedModelFile final
{

public:

	/*
	*	Mesh class definition.
	*/
	class Mesh final
	{

	public:

		//The name.
		DynamicString _Name;

		//The vertices.
		DynamicArray<AnimatedVertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

	};

	//The meshes.
	DynamicArray<Mesh> _Meshes;

	//The skeleton.
	Skeleton _Skeleton;

};