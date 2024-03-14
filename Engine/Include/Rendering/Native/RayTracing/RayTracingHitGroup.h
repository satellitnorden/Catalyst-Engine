#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Entities.
#include <Entities/Core/Entity.h>

//Rendering.
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>

class RayTracingHitGroup final
{

public:

	/*
	*	Entry class definition.
	*/
	class Entry final
	{

	public:

		//The entity identifier.
		EntityIdentifier _EntityIdentifier;

		//The vertex buffer.
		BufferHandle _VertexBuffer;

		//The index buffer.
		BufferHandle _IndexBuffer;

		//The material index.
		uint32 _MaterialIndex;

		//The instance data.
		TopLevelAccelerationStructureInstanceData _InstanceData;

	};

	/*
	*	Shared bottom level acceleration structure class definition.
	*/
	class SharedBottomLevelAccelerationStructure final
	{

	public:

		//The bottom level acceleration structure.
		AccelerationStructureHandle _BottomLevelAccelerationStructure;

		//The reference count.
		uint32 _ReferenceCount;

	};

	//The identifier.
	HashString _Identifier;

	//The index.
	uint32 _Index;

	//The bottom level acceleration structure flags.
	BottomLevelAccelerationStructureFlag _BottomLevelAccelerationStructureFlags;

	//The entries.
	DynamicArray<Entry> _Entries;

	//The material indices.
	DynamicArray<uint32> _Materialindices;

	//The material indices buffer.
	BufferHandle _MaterialIndicesBuffer{ EMPTY_HANDLE };

	//The material indices buffer capacity.
	uint64 _MaterialBufferIndicesCapacity{ 0 };

	//The shared bottom level acceleration structures.
	HashTable<HashString, SharedBottomLevelAccelerationStructure> _SharedBottomLevelAccelerationStructures;

};