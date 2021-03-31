#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Forward declarations.
class Pipeline;

class CommandBuffer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CommandBuffer() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the command buffer data.
	*/
	FORCE_INLINE CommandBuffer(void *const RESTRICT initial_command_buffer_data) NOEXCEPT
		:
		_CommandBufferData(initial_command_buffer_data)
	{

	}

	/*
	*	Returns the underlying the command buffer data.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD void *const RESTRICT GetCommandBufferData() const NOEXCEPT
	{
		return _CommandBufferData;
	}

	/*
	*	Begins the command buffer.
	*/
	void Begin(const Pipeline *const RESTRICT pipeline) NOEXCEPT;

	/*
	*	Binds an index buffer.
	*/
	void BindIndexBuffer(const Pipeline *const RESTRICT pipeline, BufferHandle buffer, const uint64 offset) NOEXCEPT;

	/*
	*	Binds a pipeline.
	*/
	void BindPipeline(const Pipeline *const RESTRICT pipeline) NOEXCEPT;

	/*
	*	Binds a render data table.
	*/
	void BindRenderDataTable(const Pipeline *const RESTRICT pipeline, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT;

	/*
	*	Binds a number of vertex buffers.
	*/
	void BindVertexBuffer(const Pipeline *const RESTRICT pipeline, const uint32 binding, BufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT;

	/*
	*	Dispatches.
	*/
	void Dispatch(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height, const uint32 depth) NOEXCEPT;

	/*
	*	Draws.
	*/
	void Draw(const Pipeline *const RESTRICT pipeline, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT;

#if defined(CATALYST_EDITOR)
	/*
	*	Draws the current ImGui state.
	*/
	void DrawImGui(const Pipeline *const RESTRICT pipeline) NOEXCEPT;
#endif

	/*
	*	Draws indexed.
	*/
	void DrawIndexed(const Pipeline *const RESTRICT pipeline, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT;

	/*
	*	Establishes an image memory barrier.
	*/
	void ImageMemoryBarrier(const Pipeline* const RESTRICT pipeline, const OpaqueHandle image) NOEXCEPT;

	/*
	*	Pushes constants.
	*/
	void PushConstants(const Pipeline *const RESTRICT pipeline, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT;

	/*
	*	Sets an event.
	*/
	void SetEvent(const Pipeline *const RESTRICT pipeline, const EventHandle event) NOEXCEPT;

	/*
	*	Traces rays.
	*/
	void TraceRays(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Writes a timestamp.
	*/
	void WriteTimestamp(const Pipeline *const RESTRICT pipeline, const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT;

	/*
	*	Ends the command buffer.
	*/
	void End(const Pipeline *const RESTRICT pipeline) NOEXCEPT;

private:

	//The underlying command buffer data.
	void *RESTRICT _CommandBufferData;

};