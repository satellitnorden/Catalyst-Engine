//Header file.
#include <Rendering/Engine Layer/Render Passes/DirectionalInstancedPhysicalShadowRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>
#include <Rendering/Engine Layer/PhysicalVertex.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DirectionalInstancedPhysicalShadowRenderPass);

/*
*	Initializes the directional instanced physical shadow render pass.
*/
void DirectionalInstancedPhysicalShadowRenderPass::Initialize() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::DirectionalInstancedPhysicalShadow);

	//Set the shaders.
	SetVertexShader(Shader::DirectionalInstancedPhysicalShadowVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::ShadowMapFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::DirectionalLight);

	//Add the render targets.
	SetNumberOfRenderTargets(1);
	AddRenderTarget(RenderTarget::DirectionalPreBlurShadowMap);

	//Add the descriptor set layouts.
	SetNumberOfDescriptorSetLayouts(2);
	AddDescriptorSetLayout(DescriptorSetLayout::DynamicUniformData);
	AddDescriptorSetLayout(DescriptorSetLayout::Physical);

	//Add the vertex input attribute descriptions.
	SetNumberOfVertexInputAttributeDescriptions(8);
	AddVertexInputAttributeDescription(0,
		0,
		VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
		offsetof(PhysicalVertex, position));
	AddVertexInputAttributeDescription(1,
		0,
		VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
		offsetof(PhysicalVertex, normal));
	AddVertexInputAttributeDescription(2,
		0,
		VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat,
		offsetof(PhysicalVertex, tangent));
	AddVertexInputAttributeDescription(3,
		0,
		VertexInputAttributeDescription::Format::X32Y32SignedFloat,
		offsetof(PhysicalVertex, textureCoordinate));
	AddVertexInputAttributeDescription(4,
		1,
		VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
		0);
	AddVertexInputAttributeDescription(5,
		1,
		VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
		sizeof(Vector4));
	AddVertexInputAttributeDescription(6,
		1,
		VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
		sizeof(Vector4) * 2);
	AddVertexInputAttributeDescription(7,
		1,
		VertexInputAttributeDescription::Format::X32Y32Z32W32SignedFloat,
		sizeof(Vector4) * 3);

	//Add the vertex input binding descriptions.
	SetNumberOfVertexInputBindingDescriptions(2);
	AddVertexInputBindingDescription(0, sizeof(PhysicalVertex), VertexInputBindingDescription::InputRate::Vertex);
	AddVertexInputBindingDescription(1, sizeof(Matrix4), VertexInputBindingDescription::InputRate::Instance);

	//Set the render resolution.
	SetRenderResolution(Resolution(RenderingConstants::SHADOW_MAP_RESOLUTION, RenderingConstants::SHADOW_MAP_RESOLUTION));

	//Set the properties of the render pass.
	SetBlendEnabled(false);
	SetColorAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetColorAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetCullMode(CullMode::Back);
	SetDepthAttachmentLoadOperator(AttachmentLoadOperator::Load);
	SetDepthAttachmentStoreOperator(AttachmentStoreOperator::Store);
	SetDepthCompareOperator(CompareOperator::Less);
	SetDepthTestEnabled(true);
	SetDepthWriteEnabled(true);
	SetTopology(Topology::TriangleList);

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the terrain.
*/
void DirectionalInstancedPhysicalShadowRenderPass::Render() NOEXCEPT
{
	//Iterate over all instanced physical components and draw them all.
	const uint64 numberOfInstancedPhysicalComponents{ ComponentManager::GetNumberOfInstancedPhysicalComponents() };

	//If there's none to render - render none.
	if (numberOfInstancedPhysicalComponents == 0)
	{
		return;
	}

	//Cache data the will be used.
	CommandBuffer *const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const DescriptorSetHandle currentDynamicUniformDataDescriptorSet{ RenderingSystem::Instance->GetCurrentDynamicUniformDataDescriptorSet() };
	const InstancedPhysicalRenderComponent *RESTRICT component{ ComponentManager::GetInstancedPhysicalRenderComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the current dynamic uniform data descriptor set.
	commandBuffer->BindDescriptorSets(this, 0, 1, &currentDynamicUniformDataDescriptorSet);

	StaticArray<uint64, 2> offsets
	{
		static_cast<uint64>(0),
		static_cast<uint64>(0)
	};

	for (uint64 i = 0; i < numberOfInstancedPhysicalComponents; ++i, ++component)
	{
		StaticArray<GraphicsBufferHandle, 2> buffers
		{
			static_cast<const VkBuffer>(component->modelBuffer),
			static_cast<const VkBuffer>(component->transformationsBuffer)
		};

		commandBuffer->BindDescriptorSets(this, 1, 1, &component->descriptorSet);
		commandBuffer->BindVertexBuffers(this, 2, buffers.Data(), offsets.Data());
		commandBuffer->BindIndexBuffer(this, component->modelBuffer, component->indexOffset);
		commandBuffer->DrawIndexed(this, component->indexCount, component->instanceCount);
	}

	//End the command buffer.
	commandBuffer->End(this);
}