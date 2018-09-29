#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

//Forward declarations.
class RenderPass;

class CommandBuffer
{

public:

	/*
	*	Begins the command buffer.
	*/
	virtual void Begin(const RenderPass *const RESTRICT _RenderPass) NOEXCEPT = 0;

	/*
	*	Binds an index buffer.
	*/
	virtual void BindIndexBuffer(const RenderPass *const RESTRICT _RenderPass, ConstantBufferHandle buffer, const uint64 offset) NOEXCEPT = 0;

	/*
	*	Binds a render data table.
	*/
	virtual void BindRenderDataTable(const RenderPass *const RESTRICT _RenderPass, const uint32 binding, const RenderDataTableHandle renderDataTable) NOEXCEPT = 0;


	/*
	*	Binds a number of vertex buffers.
	*/
	virtual void BindVertexBuffer(const RenderPass *const RESTRICT renderPass, const uint32 binding, ConstantBufferHandle buffer, const uint64 *const RESTRICT offset) NOEXCEPT = 0;

	/*
	*	Draws.
	*/
	virtual void Draw(const RenderPass *const RESTRICT _RenderPass, const uint32 vertexCount, const uint32 instanceCount) NOEXCEPT = 0;

	/*
	*	Draws indexed.
	*/
	virtual void DrawIndexed(const RenderPass *const RESTRICT _RenderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT = 0;

	/*
	*	Pushes constants.
	*/
	virtual void PushConstants(const RenderPass *const RESTRICT _RenderPass, ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT = 0;

	/*
	*	Ends the command buffer.
	*/
	virtual void End(const RenderPass *const RESTRICT _RenderPass) NOEXCEPT = 0;

};