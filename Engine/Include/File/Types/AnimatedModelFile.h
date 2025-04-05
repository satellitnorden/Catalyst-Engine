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

		//The name.
		DynamicString _Name;

		//The vertices.
		DynamicArray<AnimatedVertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

	};

	//The creator.
	Creator _Creator{ Creator::UNKNOWN };

	//The meshes.
	DynamicArray<Mesh> _Meshes;

	//The parent transform.
	Matrix4x4 _ParentTransform;

	//The skeleton.
	Skeleton _Skeleton;

	/*
	*	Post processes this animated model file.
	*/
	void PostProcess() NOEXCEPT;

};