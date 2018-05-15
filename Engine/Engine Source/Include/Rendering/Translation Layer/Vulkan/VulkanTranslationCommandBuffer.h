#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/API Layer/Vulkan/VulkanCommandBuffer.h>
#include <Rendering/Engine Layer/CommandBuffer.h>

class VulkanTranslationCommandBuffer final : public CommandBuffer
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	VulkanTranslationCommandBuffer() NOEXCEPT = delete;

	/*
	*	Begins the command buffer.
	*/
	void Begin(const RenderPass *const RESTRICT renderPass) NOEXCEPT override;

	/*
	*	Binds a number of descriptor sets.
	*/
	void BindDescriptorSets(const RenderPass *const RESTRICT renderPass, const uint32 firstBinding, const uint32 descriptorSetCount, const DescriptorSetHandle *const RESTRICT descriptorSets) NOEXCEPT override;

	/*
	*	Ends the command buffer.
	*/
	void End(const RenderPass *const RESTRICT) NOEXCEPT override;

private:

	//The underlying Vulkan command buffer.
	VulkanCommandBuffer commandBuffer;

};