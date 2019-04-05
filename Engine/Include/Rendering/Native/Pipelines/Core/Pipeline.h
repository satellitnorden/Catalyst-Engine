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

private:

	//The pipeline type.
	Type _Type;

	//The initialization function.
	InitializationFunction _InitializationFunction;

	//The execution function.
	ExecutionFunction _ExecutionFunction;

	//The command buffers.
	DynamicArray<CommandBuffer *RESTRICT> _CommandBuffers;

};