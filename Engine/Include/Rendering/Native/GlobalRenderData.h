#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Rendering/Native/RenderingPerformanceData.h>
#endif
#include <Rendering/Native/CommandBuffer.h>

/*
*	The global command pool data.
*/
class GlobalCommandPoolData final
{

public:

	/*
	*	The thread data.
	*/
	class ThreadData final
	{

	public:

		//The command pool.
		CommandPoolHandle _CommandPool;

		//The primary command buffers.
		DynamicArray<CommandBuffer *RESTRICT> _PrimaryCommandBuffers;

		//The number of primary command buffers used.
		uint64 _NumberOfPrimaryCommandBuffersUsed{ 0 };

		//The secondary command buffers.
		DynamicArray<CommandBuffer *RESTRICT> _SecondaryCommandBuffers;

		//The number of secondary command buffers used.
		uint64 _NumberOfSecondaryCommandBuffersUsed{ 0 };

	};

	//Container for all the thread data.
	DynamicArray<ThreadData> _ThreadData;

};

/*
*	The global render data.
*/
class GlobalRenderData final
{

public:

	/*
	*	Add global texture request class definition.
	*/
	class AddGlobalTextureRequest final
	{

	public:

		//The index.
		uint32 _Index;

		//The texture.
		Texture2DHandle _Texture;

		//The average value.
		Vector4<float32> _AverageValue;

	};

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The render data tables 2.
	DynamicArray<RenderDataTableHandle> _RenderDataTables2;

	//The dynamic uniform data buffers.
	DynamicArray<BufferHandle> _DynamicUniformDataBuffers;

	//The global texture lock.
	Spinlock _GlobalTexturesLock;

	//The global texture slots.
	StaticArray<bool, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES> _GlobalTextureSlots;

	//The global texture average values.
	StaticArray<Vector4<float32>, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES> _GlobalTextureAverageValues;

	//The global texture average value buffers.
	DynamicArray<BufferHandle> _GlobalTextureAverageValueBuffers;;

	//The remove global texture updates.
	DynamicArray<DynamicArray<uint32>> _RemoveGlobalTextureUpdates;

	//The add global texture updates.
	DynamicArray<DynamicArray<AddGlobalTextureRequest>> _AddGlobalTextureUpdates;

	//Container for all the global command pool data.
	DynamicArray<GlobalCommandPoolData> _GlobalCommandPoolData;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Container for all performance data.
	DynamicArray<RenderingPerformanceData> _PerformanceData;
#endif

};