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
using PreRecordFunction = void(*)(CommandBuffer *const RESTRICT frame_command_buffer);
using PostRecordFunction = void(*)(CommandBuffer *const RESTRICT frame_command_buffer);
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
	*	Pre records this render pass.
	*/
	void PreRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT;

	/*
	*	Post records this render pass.
	*/
	void PostRecord(CommandBuffer *const RESTRICT frame_command_buffer) NOEXCEPT;

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

	/*
	*	Returns the default native stage.
	*/
	FORCE_INLINE NO_DISCARD DefaultNativeRenderPassStage GetDefaultNativeStage() const NOEXCEPT
	{
		return _DefaultNativeStage;
	}

	/*
	*	Sets the default native stage.
	*/
	FORCE_INLINE void SetDefaultNativeStage(const DefaultNativeRenderPassStage value) NOEXCEPT
	{
		_DefaultNativeStage = value;
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
	*	Sets the pre record function.
	*/
	FORCE_INLINE void SetPreRecordFunction(const PreRecordFunction function) NOEXCEPT
	{
		_PreRecordFunction = function;
	}

	/*
	*	Sets the post record function.
	*/
	FORCE_INLINE void SetPostRecordFunction(const PostRecordFunction function) NOEXCEPT
	{
		_PostRecordFunction = function;
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Sets the name.
	*/
	FORCE_INLINE void SetName(const char *const RESTRICT value) NOEXCEPT
	{
		_Name = value;
	}
#endif

private:

	//Denotes if this render pass is enabled or not.
	bool _Enabled{ true };

	//The initialization function.
	InitializationFunction _InitializationFunction;

	//The execution function.
	ExecutionFunction _ExecutionFunction;

	//The pre record function.
	PreRecordFunction _PreRecordFunction{ nullptr };

	//The post record function.
	PostRecordFunction _PostRecordFunction{ nullptr };

	//The termination function.
	TerminationFunction _TerminationFunction;

	//The pipelines.
	DynamicArray<Pipeline *RESTRICT> _Pipelines;

	//The default native stage.
	DefaultNativeRenderPassStage _DefaultNativeStage;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The name.
	const char *RESTRICT _Name{ nullptr };
#endif

};