#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Type alises.
using InitializationFunction = void(*)();
using ExecutionFunction = void(*)();
using TerminationFunction = void(*)();

class RenderPass
{

public:

	/*
	*	Returns whether or not this render pass is enabled.
	*/
	bool IsEnabled() const NOEXCEPT
	{
		return _Enabled;
	}

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the number of pipelines.
	*/
	FORCE_INLINE NO_DISCARD uint64 GetNumberOfPipelines() const NOEXCEPT
	{
		return _Pipelines.Size();
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
	*	Resets this render pass.
	*/
	void ResetRenderPass() NOEXCEPT;

	/*
	*	Ses whether or not this render pass is enabled.
	*/
	void SetEnabled(const bool enabled) NOEXCEPT
	{
		_Enabled = enabled;
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
	*	Sets the termination function.
	*/
	FORCE_INLINE void SetTerminationFunction(const TerminationFunction function) NOEXCEPT
	{
		_TerminationFunction = function;
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
		_Pipelines.Emplace(pipeline);
	}

private:

	//Denotes if this render pass is enabled or not.
	bool _Enabled{ true };

	//The initialization function.
	InitializationFunction _InitializationFunction;

	//The execution function.
	ExecutionFunction _ExecutionFunction;

	//The termination function.
	TerminationFunction _TerminationFunction;

	//The pipelines.
	DynamicArray<Pipeline *RESTRICT> _Pipelines;

};