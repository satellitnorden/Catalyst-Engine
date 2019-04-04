#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Type alises.
using InitializationFunction = void(*)();
using ExecutionFunction = void(*)();

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
	*	Executes this pipeline.
	*/
	FORCE_INLINE void Execute() NOEXCEPT
	{
		_ExecutionFunction();
	}

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

};