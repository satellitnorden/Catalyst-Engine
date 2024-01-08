#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Type aliases.
using RenderInputStreamGatherFunction = void(*)(void *const RESTRICT user_data, class RenderInputStream *const RESTRICT input_stream);

/*
*	Render input stream entry class definition.
*/
class RenderInputStreamEntry final
{

public:

	//The push constant data offset
	uint64 _PushConstantDataOffset;

	union
	{
		//'DISPATCH' data.
		struct
		{
			//The dispatch width.
			uint32 _DispatchWidth;

			//The dispatch height.
			uint32 _DispatchHeight;

			//The dispatch depth.
			uint32 _DispatchDepth;
		};

		//'TRACE_RAYS' data.
		struct
		{
			//The trace rays width.
			uint32 _TraceRaysWidth;

			//The trace rays height.
			uint32 _TraceRaysHeight;
		};

		struct
		{
			//The vertex buffer.
			BufferHandle _VertexBuffer;

			//The index buffer.
			BufferHandle _IndexBuffer;

			//The index buffer offset.
			uint64 _IndexBufferOffset;

			//The instance buffer.
			BufferHandle _InstanceBuffer;

			//The vertex count.
			uint32 _VertexCount;

			//The index count.
			uint32 _IndexCount;

			//The instance count.
			uint32 _InstanceCount;
		};
	};
	

};

/*
*	Render input stream class defintion.
*/
class RenderInputStream final
{

public:

	//Enumeration covering all modes.
	enum class Mode : uint8
	{
		/*
		*	For each entry:
		*	Dispatch with '_DispatchWidth', '_DispatchHeight' and '_DispatchDepth'.
		*/
		DISPATCH,

		/*
		*	For each entry:
		*	Should just draw with '_VertexCount' amount of vertices, without binding any buffers.
		*/
		DRAW,

		/*
		*	For each entry:
		*	Should just draw with '_VertexCount' amount of vertices, with '_InstanceCount' instances using '_InstanceBuffer'.
		*/
		DRAW_INSTANCED,

		/*
		*	For each entry:
		*	Should draw indexed with the vertex/index buffer and '_IndexCount'.
		*/
		DRAW_INDEXED,

		/*
		*	For each entry:
		*	Should draw indexed with the vertex/index buffer and '_IndexCount', with '_InstanceCount' instances using '_InstanceBuffer'.
		*/
		DRAW_INDEXED_INSTANCED,

		/*
		*	For each entry:
		*	Should trace rays with '_TraceRaysWidth' and '_TraceRaysHeight'.
		*/
		TRACE_RAYS
	};

	//The identifier.
	HashString _Identifier;

	//The required vertex attribute descriptions.
	DynamicArray<VertexInputAttributeDescription> _RequiredVertexInputAttributeDescriptions;

	//The required vertex input binding descriptions.
	DynamicArray<VertexInputBindingDescription> _RequiredVertexInputBindingDescriptions;

	//The required push constant data size.
	uint64 _RequiredPushConstantDataSize;

	//The gather function.
	RenderInputStreamGatherFunction _GatherFunction;

	//The mode.
	Mode _Mode;

	//The user data.
	void *RESTRICT _UserData;

	//The entries.
	DynamicArray<RenderInputStreamEntry> _Entries;

	//The push constant data memory.
	DynamicArray<byte> _PushConstantDataMemory;

};