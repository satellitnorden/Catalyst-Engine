#if defined(CATALYST_RENDERING_OPENGL)
//Header file.
#include <Rendering/Engine/CommandBuffer.h>

/*
*	Begins the command buffer.
*/
void CommandBuffer::Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT
{
	
}

/*
*	Binds an index buffer.
*/
void CommandBuffer::BindIndexBuffer(const RenderPass *const RESTRICT renderPass, ConstantBufferHandle buffer, const uint64 offset) NOEXCEPT
{
	
}

/*
*	Binds a render data table.
*/
void CommandBuffer::BindRenderDataTable(const RenderPass *const RESTRICT renderPass, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT
{

}

/*
*	Binds a number of vertex buffers.
*/
void CommandBuffer::BindVertexBuffer(const RenderPass *const RESTRICT renderPass, const uint32 binding, ConstantBufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT
{

}

/*
*	Draws.
*/
void CommandBuffer::Draw(const RenderPass *const RESTRICT renderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT
{

}

/*
*	Draws indexed.
*/
void CommandBuffer::DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT
{

}

/*
*	Pushes constants.
*/
void CommandBuffer::PushConstants(const RenderPass *const RESTRICT renderPass, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT
{

}

/*
*	Ends the command buffer.
*/
void CommandBuffer::End(const RenderPass *const RESTRICT) NOEXCEPT
{

}
#endif