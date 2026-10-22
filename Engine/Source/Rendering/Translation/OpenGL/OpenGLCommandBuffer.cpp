#if defined(CATALYST_HAS_OPEN_GL)
//Header file.
#include <Rendering/Native/CommandBuffer.h>

/*
*	Begins the command buffer.
*/
void CommandBuffer::Begin(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	
}

/*
*	Binds an index buffer.
*/
void CommandBuffer::BindIndexBuffer(const Pipeline *const RESTRICT pipeline, BufferHandle buffer, const uint64 offset) NOEXCEPT
{
	
}

/*
*	Binds a pipeline.
*/
void CommandBuffer::BindPipeline(const Pipeline* const RESTRICT pipeline) NOEXCEPT
{
	
}

/*
*	Binds a render data table.
*/
void CommandBuffer::BindRenderDataTable(const Pipeline *const RESTRICT pipeline, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{
	
}

/*
*	Binds a number of vertex buffers.
*/
void CommandBuffer::BindVertexBuffer(const Pipeline *const RESTRICT pipeline, const uint32 binding, BufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT
{

}

/*
*	Blits an image.
*/
void CommandBuffer::BlitImage(const Pipeline *const RESTRICT pipeline, const OpaqueHandle source, const OpaqueHandle target) NOEXCEPT
{
	
}

/*
*	Clears a depth/stencil image.
*/
void CommandBuffer::ClearDepthStencilImage(const OpaqueHandle image) NOEXCEPT
{
	
}

/*
*	Clears a color image.
*/
void CommandBuffer::ClearColorImage(const OpaqueHandle image) NOEXCEPT
{
	
}

/*
*	Dispatches.
*/
void CommandBuffer::Dispatch(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height, const uint32 depth) NOEXCEPT
{
	
}

/*
*	Draws.
*/
void CommandBuffer::Draw(const Pipeline *const RESTRICT pipeline, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{
	
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Draws the current ImGui state.
*/
void CommandBuffer::DrawImGui(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	
}
#endif

/*
*	Draws indexed.
*/
void CommandBuffer::DrawIndexed(const Pipeline *const RESTRICT pipeline, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{
	
}

/*
*	Executes commands from a secondary command buffer.
*/
void CommandBuffer::ExecuteCommands(const Pipeline *const RESTRICT pipeline, const CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	
}

/*
*	Establishes an image memory barrier.
*/
void CommandBuffer::ImageMemoryBarrier(	const Pipeline* const RESTRICT pipeline,
										const AccessFlags source_access_mask,
										const AccessFlags destination_access_mask,
										const ImageLayout old_layout,
										const ImageLayout new_layout,
										const OpaqueHandle image,
										const PipelineStageFlags source_stage_mask,
										const PipelineStageFlags destination_stage_mask) NOEXCEPT
{
	
}

/*
*	Pushes constants.
*/
void CommandBuffer::PushConstants(const Pipeline *const RESTRICT pipeline, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{
	
}

/*
*	Sets an event.
*/
void CommandBuffer::SetEvent(const Pipeline *const RESTRICT pipeline, const EventHandle event) NOEXCEPT
{

}

/*
*	Traces rays.
*/
void CommandBuffer::TraceRays(const Pipeline *const RESTRICT pipeline, const uint32 width, const uint32 height) NOEXCEPT
{
	
}

/*
*	Writes a begin timestamp.
*/
void CommandBuffer::WriteBeginTimestamp(const Pipeline *const RESTRICT pipeline, const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	
}

/*
*	Writes an end timestamp.
*/
void CommandBuffer::WriteEndTimestamp(const Pipeline *const RESTRICT pipeline, const QueryPoolHandle query_pool, const uint32 query_index) NOEXCEPT
{
	
}

/*
*	Ends the command buffer.
*/
void CommandBuffer::End(const Pipeline *const RESTRICT pipeline) NOEXCEPT
{
	
}
#endif