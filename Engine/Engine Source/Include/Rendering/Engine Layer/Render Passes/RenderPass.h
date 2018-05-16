#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

//Forward declarations.
class CommandBuffer;

class RenderPass
{

public:

	/*
	*	Returns the data for this render pass.
	*/
	const void *const RESTRICT GetData() const NOEXCEPT { return data; }

	/*
	*	Returns the stage.
	*/
	RenderPassStage GetStage() const NOEXCEPT { return stage; }

	/*
	*	Returns the vertex shader.
	*/
	Shader GetVertexShader() const NOEXCEPT { return vertexShader; }

	/*
	*	Returns the tessellation control shader.
	*/
	Shader GetTessellationControlShader() const NOEXCEPT { return tessellationControlShader; }

	/*
	*	Returns the tessellation evaluation shader.
	*/
	Shader GetTessellationEvaluationShader() const NOEXCEPT { return tessellationEvaluationShader; }

	/*
	*	Returns the geometry shader.
	*/
	Shader GetGeometryShader() const NOEXCEPT { return geometryShader; }

	/*
	*	Returns the fragment shader.
	*/
	Shader GetFragmentShader() const NOEXCEPT { return fragmentShader; }

	/*
	*	Returns the depth buffer.
	*/
	DepthBuffer GetDepthBuffer() const NOEXCEPT { return depthBuffer; }

	/*
	*	Returns the render targets.
	*/
	const DynamicArray<RenderTarget>& GetRenderTargets() const NOEXCEPT { return renderTargets; }

	/*
	*	Returns the descriptor set layouts.
	*/
	const DynamicArray<DescriptorSetLayout>& GetDescriptorSetLayouts() const NOEXCEPT { return descriptorSetLayouts; }

	/*
	*	Returns the vertex input attribute descriptions.
	*/
	const DynamicArray<VertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const NOEXCEPT { return vertexInputAttributeDescriptions; }

	/*
	*	Returns the vertex input binding descriptions.
	*/
	const DynamicArray<VertexInputBindingDescription>& GetVertexInputBindingDescriptions() const NOEXCEPT { return vertexInputBindingDescriptions; }

	/*
	*	Returns whether or not blend is enabled.
	*/
	bool IsBlendEnabled() const NOEXCEPT { return blendEnabled; }

	/*
	*	Returns the color attachment load operator.
	*/
	AttachmentLoadOperator GetColorAttachmentLoadOperator() const NOEXCEPT { return colorAttachmentLoadOperator; }

	/*
	*	Returns the color attachment store operator.
	*/
	AttachmentStoreOperator GetColorAttachmentStoreOperator() const NOEXCEPT { return colorAttachmentStoreOperator; }

	/*
	*	Returns the cull mode.
	*/
	CullMode GetCullMode() const NOEXCEPT { return cullMode; }

	/*
	*	Returns the depth attachment load operator.
	*/
	AttachmentLoadOperator GetDepthAttachmentLoadOperator() const NOEXCEPT { return depthAttachmentLoadOperator; }

	/*
	*	Returns the depth attachment store operator.
	*/
	AttachmentStoreOperator GetDepthAttachmentStoreOperator() const NOEXCEPT { return depthAttachmentStoreOperator; }

	/*
	*	Returns the depth compare operator.
	*/
	CompareOperator GetDepthCompareOperator() const NOEXCEPT { return depthCompareOperator; }

	/*
	*	Returns whether or not depth test is enabled.
	*/
	bool IsDepthTestEnabled() const NOEXCEPT { return depthTestEnabled; }

	/*
	*	Returns whether or not depth write is enabled.
	*/
	bool IsDepthWriteEnabled() const NOEXCEPT { return depthWriteEnabled; }

	/*
	*	Returns the topology.
	*/
	Topology GetTopology() const NOEXCEPT { return topology; }

protected:

	/*
	*	Sets the stage.
	*/
	void SetStage(const RenderPassStage newStage) NOEXCEPT { stage = newStage; }

	/*
	*	Sets the vertex shader.
	*/
	void SetVertexShader(const Shader newVertexShader) NOEXCEPT { vertexShader = newVertexShader; }

	/*
	*	Sets the tessellation control shader.
	*/
	void SetTessellationControlShader(const Shader newTessellationControlShader) NOEXCEPT { tessellationControlShader = newTessellationControlShader; }

	/*
	*	Sets the tessellation evaluation shader.
	*/
	void SetTessellationEvaluationShader(const Shader newTessellationEvaluationShader) NOEXCEPT { tessellationEvaluationShader = newTessellationEvaluationShader; }

	/*
	*	Sets the geometry shader.
	*/
	void SetGeometryShader(const Shader newGeometryShader) NOEXCEPT { geometryShader = newGeometryShader; }

	/*
	*	Sets the fragment shader.
	*/
	void SetFragmentShader(const Shader newFragmentShader) NOEXCEPT { fragmentShader = newFragmentShader; }

	/*
	*	Sets the depth buffer.
	*/
	void SetDepthBuffer(const DepthBuffer newDepthBuffer) NOEXCEPT { depthBuffer = newDepthBuffer; }

	/*
	*	Sets the number of render targets.
	*/
	void SetNumberOfRenderTargets(const uint64 numberOfRenderTargets) NOEXCEPT { renderTargets.Reserve(numberOfRenderTargets); }

	/*
	*	Adds a render target.
	*/
	void AddRenderTarget(const RenderTarget newRenderTarget) NOEXCEPT { renderTargets.EmplaceFast(newRenderTarget); }

	/*
	*	Sets the number of descriptor set layouts.
	*/
	void SetNumberOfDescriptorSetLayouts(const uint64 numberODescriptorSetLayouts) NOEXCEPT { descriptorSetLayouts.Reserve(numberODescriptorSetLayouts); }

	/*
	*	Adds a descriptor set layout.
	*/
	void AddDescriptorSetLayout(const DescriptorSetLayout newDescriptorSetLayout) NOEXCEPT { descriptorSetLayouts.EmplaceFast(newDescriptorSetLayout); }

	/*
	*	Sets the number of vertex input attribute descriptions.
	*/
	void SetNumberOfVertexInputAttributeDescriptions(const uint64 numberOfVertexInputAttributeDescriptions) NOEXCEPT { vertexInputAttributeDescriptions.Reserve(numberOfVertexInputAttributeDescriptions); }

	/*
	*	Adds a vertex input attribute description.
	*/
	void AddVertexInputAttributeDescription(const uint32 location, const uint32 binding, const VertexInputAttributeDescription::Format format, const uint32 offset) NOEXCEPT { vertexInputAttributeDescriptions.EmplaceFast(location, binding, format, offset); }

	/*
	*	Sets the number of vertex input binding descriptions.
	*/
	void SetNumberOfVertexInputBindingDescriptions(const uint64 numberOfVertexInputBindingDescriptions) NOEXCEPT { vertexInputBindingDescriptions.Reserve(numberOfVertexInputBindingDescriptions); }

	/*
	*	Adds a vertex input binding description.
	*/
	void AddVertexInputBindingDescription(const uint32 binding, const uint32 stride, const VertexInputBindingDescription::InputRate inputRate) NOEXCEPT { vertexInputBindingDescriptions.EmplaceFast(binding, stride, inputRate); }

	/*
	*	Sets whether or not blend is enabled.
	*/
	void SetBlendEnabled(const bool newBlendEnabled) NOEXCEPT { blendEnabled = newBlendEnabled; }

	/*
	*	Sets the color attachment load operator.
	*/
	void SetColorAttachmentLoadOperator(const AttachmentLoadOperator newColorAttachmentLoadOperator) NOEXCEPT { colorAttachmentLoadOperator = newColorAttachmentLoadOperator; }

	/*
	*	Sets the color attachment store operator.
	*/
	void SetColorAttachmentStoreOperator(const AttachmentStoreOperator newColorAttachmentStorOperator) NOEXCEPT { colorAttachmentStoreOperator = newColorAttachmentStorOperator; }

	/*
	*	Sets the cull mode.
	*/
	void SetCullMode(const CullMode newCullMode) NOEXCEPT { cullMode = newCullMode; }

	/*
	*	Sets the depth attachment load operator.
	*/
	void SetDepthAttachmentLoadOperator(const AttachmentLoadOperator newDepthAttachmentLoadOperator) NOEXCEPT { depthAttachmentLoadOperator = newDepthAttachmentLoadOperator; }

	/*
	*	Sets the depth attachment store operator.
	*/
	void SetDepthAttachmentStoreOperator(const AttachmentStoreOperator newDepthAttachmentStorOperator) NOEXCEPT { depthAttachmentStoreOperator = newDepthAttachmentStorOperator; }

	/*
	*	Sets the depth compare operator.
	*/
	void SetDepthCompareOperator(const CompareOperator newDepthCompareOperator) NOEXCEPT { depthCompareOperator = newDepthCompareOperator; }

	/*
	*	Sets whether or not depth test is enabled.
	*/
	void SetDepthTestEnabled(const bool newDepthTestEnabled) NOEXCEPT { depthTestEnabled = newDepthTestEnabled; }

	/*
	*	Sets whether or not depth write is enabled.
	*/
	void SetDepthWriteEnabled(const bool newDepthWriteEnabled) NOEXCEPT { depthWriteEnabled = newDepthWriteEnabled; }

	/*
	*	Sets the topology.
	*/
	void SetTopology(const Topology newTopology) NOEXCEPT { topology = newTopology; }

	/*
	*	Finalizes the initialization of a render pass.
	*/
	void FinalizeInitialization() NOEXCEPT;

	/*
	*	Returns the current command buffer.
	*/
	CommandBuffer *const RESTRICT GetCurrentCommandBuffer() NOEXCEPT;

private:

	//The data for this render pass.
	void *RESTRICT data;

	//The stage.
	RenderPassStage stage;

	//The vertex shader.
	Shader vertexShader;

	//The tessellation control shader.
	Shader tessellationControlShader;

	//The tessellation evaluation shader.
	Shader tessellationEvaluationShader;

	//The geometry shader.
	Shader geometryShader;

	//The fragment shader.
	Shader fragmentShader;

	//The depth buffer.
	DepthBuffer depthBuffer;

	//The render targets.
	DynamicArray<RenderTarget> renderTargets;

	//The descriptor set layouts.
	DynamicArray<DescriptorSetLayout> descriptorSetLayouts;

	//The vertex input attribute descriptions.
	DynamicArray<VertexInputAttributeDescription> vertexInputAttributeDescriptions;

	//The vertex input binding descriptions.
	DynamicArray<VertexInputBindingDescription> vertexInputBindingDescriptions;

	//Denotes whether or not blend is enabled.
	bool blendEnabled;

	//The color attachment load operator.
	AttachmentLoadOperator colorAttachmentLoadOperator;

	//The color attachment store operator.
	AttachmentStoreOperator colorAttachmentStoreOperator;
	
	//The cull mode.
	CullMode cullMode;

	//The depth attachment load operator.
	AttachmentLoadOperator depthAttachmentLoadOperator;

	//The depth attachment store operator.
	AttachmentStoreOperator depthAttachmentStoreOperator;

	//The depth compare operator.
	CompareOperator depthCompareOperator;

	//Denotes whether or not depth test is enabled.
	bool depthTestEnabled;

	//Denotes whether or not depth write is enabled.
	bool depthWriteEnabled;

	//The topology.
	Topology topology;

	//The command buffers.
	DynamicArray<CommandBuffer *RESTRICT> commandBuffers;

};