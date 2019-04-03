#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

class RenderPass
{

public:

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

	//The data.
	void *RESTRICT _Data;

	//The pipelines.
	DynamicArray<Pipeline *RESTRICT> _Pipelines;

};