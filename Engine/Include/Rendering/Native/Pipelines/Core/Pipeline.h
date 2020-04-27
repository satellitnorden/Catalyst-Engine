#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

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
	*	Post-initializes this pipeline.
	*/
	void PostInitialize() NOEXCEPT;

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
	*	Returns the command buffer, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const CommandBuffer *const RESTRICT GetCommandBuffer() const NOEXCEPT
	{
		return _CommandBuffer;
	}

	/*
	*	Returns the command buffer, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT GetCommandBuffer() NOEXCEPT
	{
		return _CommandBuffer;
	}

	/*
	*	Sets the command buffer.
	*/
	FORCE_INLINE void SetCommandBuffer(CommandBuffer *const RESTRICT value) NOEXCEPT
	{
		_CommandBuffer = value;
	}

	/*
	*	Returns whether or not this pipeline should be included in the final render.
	*/
	bool IncludeInRender() const NOEXCEPT { return _IncludeInRender; }

	/*
	*	Sets whether or not this render pass should be included in the final render.
	*/
	void SetIncludeInRender(const bool newIncludeInRender) NOEXCEPT { _IncludeInRender = newIncludeInRender; }

protected:

	/*
	*	Sets the type.
	*/
	FORCE_INLINE void SetType(const Type type) NOEXCEPT
	{
		_Type = type;
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
		_RenderDataTableLayouts.Emplace(newRenderDataTableLayout);
	}

	/*
	*	Sets the number of push constant ranges.
	*/
	void SetNumberOfPushConstantRanges(const uint64 numberOfPushConstantRanges) NOEXCEPT { _PushConstantRanges.Reserve(numberOfPushConstantRanges); }

	/*
	*	Adds a push constant range.
	*/
	void AddPushConstantRange(const ShaderStage shaderStage, const uint32 offset, const uint32 size) NOEXCEPT { _PushConstantRanges.Emplace(shaderStage, offset, size); }

private:

	//The pipeline type.
	Type _Type;

	//The data for this pipeline.
	const void *RESTRICT _Data;

	//The render data table layouts.
	DynamicArray<RenderDataTableLayoutHandle> _RenderDataTableLayouts;

	//The push constant ranges.
	DynamicArray<PushConstantRange> _PushConstantRanges;

	//The command buffer.
	CommandBuffer *RESTRICT _CommandBuffer{ nullptr };

	//Denotes whether or not this pipeline should be included in the final render.
	bool _IncludeInRender;

};