#pragma once

//Core.
#include <Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>
#include <Rendering/Engine/Resolution.h>

//Forward declarations.
class CommandBuffer;

class RenderPass
{

public:

	/*
	*	Default constructor.
	*/
	RenderPass() NOEXCEPT;

	/*
	*	Initializes this render pass asynchronously.
	*/
	void InitializeAsynchronous() NOEXCEPT;

	/*
	*	Waits for the initialization this render pass to finish.
	*/
	void WaitForInitialization() const NOEXCEPT;

	/*
	*	Renders this render pass asynchronously.
	*/
	void RenderAsynchronous() NOEXCEPT;

	/*
	*	Waits for the render this render pass to finish.
	*/
	void WaitForRender() const NOEXCEPT;

	/*
	*	Returns the data for this render pass.
	*/
	const void *const RESTRICT GetData() const NOEXCEPT { return data; }

	/*
	*	Sets the data for this render pass.
	*/
	void SetData(const void *const RESTRICT newData) NOEXCEPT { data = newData; }

	/*
	*	Returns the main stage.
	*/
	RenderPassMainStage GetMainStage() const NOEXCEPT { return mainStage; }

	/*
	*	Returns the sub stage.
	*/
	RenderPassSubStage GetSubStage() const NOEXCEPT { return subStage; }

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
	*	Returns the render data table layouts.
	*/
	const DynamicArray<RenderDataTableLayoutHandle>& GetRenderDataTableLayouts() const NOEXCEPT { return renderDataTableLayouts; }

	/*
	*	Returns the push constant ranges.
	*/
	const DynamicArray<PushConstantRange>& GetPushConstantRanges() const NOEXCEPT { return pushConstantRanges; }

	/*
	*	Returns the vertex input attribute descriptions.
	*/
	const DynamicArray<VertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const NOEXCEPT { return vertexInputAttributeDescriptions; }

	/*
	*	Returns the vertex input binding descriptions.
	*/
	const DynamicArray<VertexInputBindingDescription>& GetVertexInputBindingDescriptions() const NOEXCEPT { return vertexInputBindingDescriptions; }

	/*
	*	Returns the render resolution.
	*/
	Resolution GetRenderResolution() const NOEXCEPT { return renderResolution; }

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

	/*
	*	Sets the number of command buffers.
	*/
	void SetNumberOfCommandBuffers(const uint64 numberOfCommandBuffers) NOEXCEPT { commandBuffers.Reserve(numberOfCommandBuffers); }

	/*
	*	Adds a command buffer.
	*/
	void AddCommandBuffer(CommandBuffer *const RESTRICT newCommandBuffer) NOEXCEPT { commandBuffers.EmplaceFast(newCommandBuffer); }

	/*
	*	Returns whether or not this render pass should be included in the final render.
	*/
	bool IncludeInRender() const NOEXCEPT { return includeInRender; }

	/*
	*	Returns the current command buffer, non-const.
	*/
	RESTRICTED const CommandBuffer *const RESTRICT GetCurrentCommandBuffer() const NOEXCEPT;

	/*
	*	Returns the current command buffer, non-const.
	*/
	RESTRICTED CommandBuffer *const RESTRICT GetCurrentCommandBuffer() NOEXCEPT;

protected:

	/*
	*	Sets the main stage.
	*/
	void SetMainStage(const RenderPassMainStage newMainStage) NOEXCEPT { mainStage = newMainStage; }

	/*
	*	Sets the sub stage.
	*/
	void SetSubStage(const RenderPassSubStage newSubStage) NOEXCEPT { subStage = newSubStage; }

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
	*	Sets the number of render data table layouts.
	*/
	void SetNumberOfRenderDataTableLayouts(const uint64 numberOfRenderDataTableLayouts) NOEXCEPT { renderDataTableLayouts.Reserve(numberOfRenderDataTableLayouts); }

	/*
	*	Adds a render data table layout.
	*/
	void AddRenderDataTableLayout(RenderDataTableLayoutHandle newRenderDataTableLayout) NOEXCEPT { renderDataTableLayouts.EmplaceFast(newRenderDataTableLayout); }

	/*
	*	Sets the number of push constant ranges.
	*/
	void SetNumberOfPushConstantRanges(const uint64 numberOfPushConstantRanges) NOEXCEPT { pushConstantRanges.Reserve(numberOfPushConstantRanges); }

	/*
	*	Adds a push constant range.
	*/
	void AddPushConstantRange(const ShaderStage shaderStage, const uint32 offset, const uint32 size) NOEXCEPT { pushConstantRanges.EmplaceFast(shaderStage, offset, size); }

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
	*	Sets the render resolution.
	*/
	void SetRenderResolution(const Resolution &newRenderResolution) NOEXCEPT { renderResolution = newRenderResolution; }

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
	*	Sets whether or not this render pass should be included in the final render.
	*/
	void SetIncludeInRender(const bool newIncludeInRender) NOEXCEPT { includeInRender = newIncludeInRender; }

	/*
	*	Sets the initialization function.
	*/
	void SetInitializationFunction(const TaskFunction newInitializationFunction) NOEXCEPT { initializationTask.function = newInitializationFunction; }

	/*
	*	Sets the render function.
	*/
	void SetRenderFunction(const TaskFunction newRenderFunction) NOEXCEPT { renderTask.function = newRenderFunction; }

private:

	//The data for this render pass.
	const void *RESTRICT data;

	//The main stage.
	RenderPassMainStage mainStage;

	//The sub stage.
	RenderPassSubStage subStage;

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

	//The render data table layouts.
	DynamicArray<RenderDataTableLayoutHandle> renderDataTableLayouts;

	//The push constant ranges.
	DynamicArray<PushConstantRange> pushConstantRanges;

	//The vertex input attribute descriptions.
	DynamicArray<VertexInputAttributeDescription> vertexInputAttributeDescriptions;

	//The vertex input binding descriptions.
	DynamicArray<VertexInputBindingDescription> vertexInputBindingDescriptions;

	//The render resolution.
	Resolution renderResolution;

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

	//Denotes whether or not this render pass should be included in the final render.
	bool includeInRender;

	//The command buffers.
	DynamicArray<CommandBuffer *RESTRICT> commandBuffers;

	//The initialization task.
	Task initializationTask;

	//The render task.
	Task renderTask;

};