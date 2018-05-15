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
	*	Returns the cull face.
	*/
	CullFace GetCullFace() const NOEXCEPT { return cullFace; }

	/*
	*	Returns whether or not depth test is enabled.
	*/
	bool IsDepthTestEnabled() const NOEXCEPT { return depthTestEnabled; }

	/*
	*	Returns whether or not depth write is enabled.
	*/
	bool IsDepthWriteEnabled() const NOEXCEPT { return depthWriteEnabled; }

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
	*	Sets the maximum number of render targets.
	*/
	void SetMaximumNumberOfRenderTargets(const uint64 maximumNumberOfRenderTargets) NOEXCEPT { renderTargets.Reserve(maximumNumberOfRenderTargets); }

	/*
	*	Adds a render target.
	*/
	void AddRenderTarget(const RenderTarget newRenderTarget) NOEXCEPT { renderTargets.EmplaceFast(newRenderTarget); }

	/*
	*	Sets the cull face.
	*/
	void SetCullFace(const CullFace newCullFace) NOEXCEPT { cullFace = newCullFace; }

	/*
	*	Sets whether or not depth test is enabled.
	*/
	void SetDepthTestEnabled(const bool newDepthTestEnabled) NOEXCEPT { depthTestEnabled = newDepthTestEnabled; }

	/*
	*	Sets whether or not depth write is enabled.
	*/
	void SetDepthWriteEnabled(const bool newDepthWriteEnabled) NOEXCEPT { depthWriteEnabled = newDepthWriteEnabled; }

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

	//The cull face.
	CullFace cullFace;

	//Denotes whether or not depth test is enabled.
	bool depthTestEnabled;

	//Denotes whether or not depth write is enabled.
	bool depthWriteEnabled;

	//The command buffers.
	DynamicArray<CommandBuffer *RESTRICT> commandBuffers;

};