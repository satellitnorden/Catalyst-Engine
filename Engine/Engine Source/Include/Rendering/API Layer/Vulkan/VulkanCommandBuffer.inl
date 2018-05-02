#pragma once

/*
*	Records a begin render pass command and clears.
*/
template <uint32 NumberOfClearValues>
void VulkanCommandBuffer::CommandBeginRenderPassAndClear(const VulkanRenderPass &vulkanRenderPass, const uint64 framebufferIndex, const VkExtent2D renderArea, const VkSubpassContents contents) NOEXCEPT
{
	StaticArray<VkClearValue, NumberOfClearValues> clearValues;

	for (uint32 i = 0; i < NumberOfClearValues; ++i)
	{
		clearValues[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		clearValues[i].depthStencil = { 1.0f, 0 };
	}

	VkRenderPassBeginInfo renderPassBeginInfo;

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = vulkanRenderPass.Get();
	renderPassBeginInfo.framebuffer = vulkanRenderPass.GetFrameBuffers()[framebufferIndex].Get();
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = renderArea;
	renderPassBeginInfo.clearValueCount = NumberOfClearValues;
	renderPassBeginInfo.pClearValues = clearValues.Data();

	vkCmdBeginRenderPass(vulkanCommandBuffer, &renderPassBeginInfo, contents);
}