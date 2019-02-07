#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Forward declarations.
class RenderPass;

class CommandBuffer final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	CommandBuffer() NOEXCEPT = delete;

	/*
	*	Constructor taking the command buffer data.
	*/
	CommandBuffer(void *const RESTRICT commandBufferData) NOEXCEPT
		:
		_CommandBufferData(commandBufferData)
	{

	}

	/*
	*	Returns the underlying the command buffer data.
	*/
	RESTRICTED NO_DISCARD void *const RESTRICT GetCommandBufferData() const NOEXCEPT { return _CommandBufferData; }

	/*
	*	Begins the command buffer.
	*/
	void Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT;

	/*
	*	Binds an index buffer.
	*/
	void BindIndexBuffer(const RenderPass *const RESTRICT renderPass, ConstantBufferHandle buffer, const uint64 offset) NOEXCEPT;

	/*
	*	Binds a render data table.
	*/
	void BindRenderDataTable(const RenderPass *const RESTRICT renderPass, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT;

	/*
	*	Binds a number of vertex buffers.
	*/
	void BindVertexBuffer(const RenderPass *const RESTRICT renderPass, const uint32 binding, ConstantBufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT;

	/*
	*	Draws.
	*/
	void Draw(const RenderPass *const RESTRICT renderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT;

	/*
	*	Draws indexed.
	*/
	void DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT;

	/*
	*	Pushes constants.
	*/
	void PushConstants(const RenderPass *const RESTRICT renderPass, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT;

	/*
	*	Ends the command buffer.
	*/
	void End(const RenderPass *const RESTRICT) NOEXCEPT;

private:

	//The underlying command buffer data.
	void *RESTRICT _CommandBufferData;

};