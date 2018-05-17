#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

//Forward declarations.
class RenderPass;

class CommandBuffer
{

public:

	/*
	*	Begins the command buffer.
	*/
	virtual void Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT = 0;

	/*
	*	Binds a number of descriptor sets.
	*/
	virtual void BindDescriptorSets(const RenderPass *const RESTRICT renderPass, const uint32 firstBinding, const uint32 descriptorSetCount, const DescriptorSetHandle *const RESTRICT descriptorSets) NOEXCEPT = 0;

	/*
	*	Binds an index buffer.
	*/
	virtual void BindIndexBuffer(const RenderPass *const RESTRICT renderPass, const GraphicsBufferHandle indexBuffer, const uint64 offset) NOEXCEPT = 0;

	/*
	*	Binds a number of vertex buffers.
	*/
	virtual void BindVertexBuffers(const RenderPass *const RESTRICT renderPass, const uint32 numberOfVertexBuffers, const GraphicsBufferHandle *const RESTRICT vertexBuffers, const uint64 *const RESTRICT offsets) NOEXCEPT = 0;

	/*
	*	Draws indexed.
	*/
	virtual void DrawIndexed(const RenderPass *const RESTRICT renderPass, const uint32 indexCount, const uint32 instanceCount) NOEXCEPT = 0;

	/*
	*	Pushes constants.
	*/
	virtual void PushConstants(const RenderPass *const RESTRICT renderPass, PushConstantRange::ShaderStage shaderStage, const uint32 offset, const uint32 size, const void *const RESTRICT data) NOEXCEPT = 0;

	/*
	*	Ends the command buffer.
	*/
	virtual void End(const RenderPass *const RESTRICT renderPass) NOEXCEPT = 0;

};