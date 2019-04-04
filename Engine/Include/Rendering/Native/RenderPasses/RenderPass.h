#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Type alises.
using InitializationFunction = void(*)();
using ExecutionFunction = void(*)();

class RenderPass
{

public:

	/*
	*	Initializes this render pass.
	*/
	FORCE_INLINE void Initialize() NOEXCEPT
	{
		_InitializationFunction();
	}

	/*
	*	Executes this render pass.
	*/
	FORCE_INLINE void Execute() NOEXCEPT
	{
		_ExecutionFunction();
	}

	/*
	*	Returns the stage.
	*/
	FORCE_INLINE NO_DISCARD RenderPassStage GetStage() const NOEXCEPT
	{
		return _Stage;
	}

	/*
	*	Returns the data.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const void *const RESTRICT GetData() const NOEXCEPT
	{
		return _Data;
	}

	/*
	*	Sets the data.
	*/
	FORCE_INLINE void SetData(void *const RESTRICT data) NOEXCEPT
	{
		_Data = data;
	}

	/*
	*	Returns the pipelines.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Pipeline *RESTRICT>& GetPipelines() const NOEXCEPT
	{
		return _Pipelines;
	}

protected:

	/*
	*	Sets the stage.
	*/
	void SetStage(const RenderPassStage stage) NOEXCEPT;

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
	*	Sets the number of pipelines.
	*/
	FORCE_INLINE void SetNumberOfPipelines(const uint64 numberOfPipelines) NOEXCEPT
	{
		_Pipelines.Reserve(numberOfPipelines);
	}

	/*
	*	Adds a pipeline.
	*/
	FORCE_INLINE void AddPipeline(Pipeline *const RESTRICT pipeline) NOEXCEPT
	{
		_Pipelines.EmplaceFast(pipeline);
	}

private:

	//The stage.
	RenderPassStage _Stage;

	//The initialization function.
	InitializationFunction _InitializationFunction;

	//The execution function.
	ExecutionFunction _ExecutionFunction;

	//The data.
	void *RESTRICT _Data;

	//The pipelines.
	DynamicArray<Pipeline *RESTRICT> _Pipelines;

};