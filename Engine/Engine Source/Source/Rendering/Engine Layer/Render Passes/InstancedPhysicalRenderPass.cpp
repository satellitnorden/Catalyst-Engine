//Header file.
#include <Rendering/Engine Layer/Render Passes/InstancedPhysicalRenderPass.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine Layer/CommandBuffer.h>
#include <Rendering/Engine Layer/PhysicalVertex.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InstancedPhysicalRenderPass);

/*
*	Default constructor.
*/
InstancedPhysicalRenderPass::InstancedPhysicalRenderPass() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::InstancedPhysical);

	//Set the initialization function.
	SetInitializationFunction([](void *const RESTRICT)
	{
		InstancedPhysicalRenderPass::Instance->InitializeInternal();
	});
}

/*
*	Initializes the instanced physical render pass.
*/
void InstancedPhysicalRenderPass::InitializeInternal() NOEXCEPT
{
	//Set the stage.
	SetStage(RenderPassStage::InstancedPhysical);

	//Set the shaders.
	SetVertexShader(Shader::InstancedPhysicalVertex);
	SetTessellationControlShader(Shader::None);
	SetTessellationEvaluationShader(Shader::None);
	SetGeometryShader(Shader::None);
	SetFragmentShader(Shader::PhysicalFragment);

	//Set the depth buffer.
	SetDepthBuffer(DepthBuffer::SceneBuffer);

	//Add the render targets.
	SetNumberOfRenderTargets(3);
	AddRenderTarget(RenderTarget::SceneBufferAlbedo);
	AddRenderTarget(RenderTarget::SceneBufferNormalDepth);
	AddRenderTarget(RenderTarget::SceneBufferMaterialProperties);

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
	SetRenderResolution(RenderingSystem::Instance->GetRenderResolution());

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

	//Set the render function.
	SetRenderFunction([](void *const RESTRICT)
	{
		InstancedPhysicalRenderPass::Instance->RenderInternal();
	});

	//Finalize the initialization.
	FinalizeInitialization();
}

/*
*	Renders the terrain.
*/
void InstancedPhysicalRenderPass::RenderInternal() NOEXCEPT
{
	//Iterate over all instanced physical components and draw them all.
	const uint64 numberOfInstancedPhysicalComponents{ ComponentManager::GetNumberOfInstancedPhysicalComponents() };

	//If there's none to render - render none.
	if (numberOfInstancedPhysicalComponents == 0)
	{
		//Don't include this render pass in the final render.
		SetIncludeInRender(false);

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
		StaticArray<ConstantBufferHandle, 2> buffers
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

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}