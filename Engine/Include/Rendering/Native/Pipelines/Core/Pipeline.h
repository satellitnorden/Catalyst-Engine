#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Type alises.
using InitializationFunction = void(*)();
using ExecutionFunction = void(*)();

//Forward declarations.
class CommandBuffer;

class Pipeline
{

public:

	//Enumeration covering all pipeline types.
	enum class Type : uint8
	{
		Compute,
		Graphics,
		RayTracing
	};

	/*
	*	Returns the pipeline type.
	*/
	NO_DISCARD Type GetType() const NOEXCEPT
	{
		return _Type;
	}

	/*
	*	Initializes this pipeline.
	*/
	FORCE_INLINE void Initialize() NOEXCEPT
	{
		_InitializationFunction();
	}

	/*
	*	Post-initializes this pipeline.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Executes this pipeline.
	*/
	FORCE_INLINE void Execute() NOEXCEPT
	{
		_ExecutionFunction();
	}

	/*
	*	Returns the data for this pipeline.
	*/
	FORCE_INLINE const void *const RESTRICT GetData() const NOEXCEPT { return _Data; }

	/*
	*	Sets the data for this pipeline.
	*/
	FORCE_INLINE void SetData(const void *const RESTRICT newData) NOEXCEPT { _Data = newData; }

	/*
	*	Returns the render data table layouts.
	*/
	const DynamicArray<RenderDataTableLayoutHandle>& GetRenderDataTableLayouts() const NOEXCEPT { return _RenderDataTableLayouts; }

	/*
	*	Returns the push constant ranges.
	*/
	const DynamicArray<PushConstantRange>& GetPushConstantRanges() const NOEXCEPT { return _PushConstantRanges; }

	/*
	*	Sets the number of command buffers.
	*/
	FORCE_INLINE void SetNumberOfCommandBuffers(const uint64 numberOfCommandBuffers) NOEXCEPT
	{
		_CommandBuffers.Reserve(numberOfCommandBuffers);
	}

	/*
	*	Adds a command buffer.
	*/
	FORCE_INLINE void AddCommandBuffer(CommandBuffer *const RESTRICT newCommandBuffer) NOEXCEPT
	{
		_CommandBuffers.EmplaceFast(newCommandBuffer);
	}

	/*
	*	Returns the current command buffer, const.
	*/
	RESTRICTED NO_DISCARD const CommandBuffer *const RESTRICT GetCurrentCommandBuffer() const NOEXCEPT
	{
		return const_cast<Pipeline *const RESTRICT>(this)->GetCurrentCommandBuffer();
	}

	/*
	*	Returns the current command buffer, non-const.
	*/
	RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT GetCurrentCommandBuffer() NOEXCEPT;

	/*
	*	Returns whether or not this pipeline should be included in the final render.
	*/
	bool IncludeInRender() const NOEXCEPT { return _IncludeInRender; }

protected:

	/*
	*	Sets the type.
	*/
	FORCE_INLINE void SetType(const Type type) NOEXCEPT
	{
		_Type = type;
	}

	/*
	*	Sets the initialization function.
	*/
	FORCE_INLINE void SetInitializationFunction(const InitializationFunction function) NOEXCEPT
	{
		_InitializationFunction = function;
	}

	/*
	*	Sets the execution function.
	*/
	FORCE_INLINE void SetExecutionFunction(const ExecutionFunction function) NOEXCEPT
	{
		_ExecutionFunction = function;
	}

	/*
	*	Sets the number of render data table layouts.
	*/
	FORCE_INLINE void SetNumberOfRenderDataTableLayouts(const uint64 numberOfRenderDataTableLayouts) NOEXCEPT
	{
		_RenderDataTableLayouts.Reserve(numberOfRenderDataTableLayouts);
	}

	/*
	*	Adds a render data table layout.
	*/
	FORCE_INLINE void AddRenderDataTableLayout(RenderDataTableLayoutHandle newRenderDataTableLayout) NOEXCEPT
	{
		_RenderDataTableLayouts.EmplaceFast(newRenderDataTableLayout);
	}

	/*
	*	Sets the number of push constant ranges.
	*/
	void SetNumberOfPushConstantRanges(const uint64 numberOfPushConstantRanges) NOEXCEPT { _PushConstantRanges.Reserve(numberOfPushConstantRanges); }

	/*
	*	Adds a push constant range.
	*/
	void AddPushConstantRange(const ShaderStage shaderStage, const uint32 offset, const uint32 size) NOEXCEPT { _PushConstantRanges.EmplaceFast(shaderStage, offset, size); }

	/*
	*	Sets whether or not this render pass should be included in the final render.
	*/
	void SetIncludeInRender(const bool newIncludeInRender) NOEXCEPT { _IncludeInRender = newIncludeInRender; }

private:

	//The pipeline type.
	Type _Type;

	//The initialization function.
	InitializationFunction _InitializationFunction;

	//The execution function.
	ExecutionFunction _ExecutionFunction;

	//The data for this pipeline.
	const void *RESTRICT _Data;

	//The render data table layouts.
	DynamicArray<RenderDataTableLayoutHandle> _RenderDataTableLayouts;

	//The push constant ranges.
	DynamicArray<PushConstantRange> _PushConstantRanges;

	//The command buffers.
	DynamicArray<CommandBuffer *RESTRICT> _CommandBuffers;

	//Denotes whether or not this pipeline should be included in the final render.
	bool _IncludeInRender;

};