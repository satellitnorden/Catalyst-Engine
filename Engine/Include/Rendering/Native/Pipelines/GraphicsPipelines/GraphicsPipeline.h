#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>

class GraphicsPipeline : public Pipeline
{

public:

	/*
	*	Default constructor.
	*/
	GraphicsPipeline() NOEXCEPT;

	/*
	*	Returns the vertex shader.
	*/
	ResourcePointer<ShaderResource> GetVertexShader() const NOEXCEPT { return _VertexShader; }

	/*
	*	Returns the tessellation control shader.
	*/
	ResourcePointer<ShaderResource> GetTessellationControlShader() const NOEXCEPT { return _TessellationControlShader; }

	/*
	*	Returns the tessellation evaluation shader.
	*/
	ResourcePointer<ShaderResource> GetTessellationEvaluationShader() const NOEXCEPT { return _TessellationEvaluationShader; }

	/*
	*	Returns the geometry shader.
	*/
	ResourcePointer<ShaderResource> GetGeometryShader() const NOEXCEPT { return _GeometryShader; }

	/*
	*	Returns the fragment shader.
	*/
	ResourcePointer<ShaderResource> GetFragmentShader() const NOEXCEPT { return _FragmentShader; }

	/*
	*	Returns the depth buffer.
	*/
	DepthBufferHandle GetDepthBuffer() const NOEXCEPT { return _DepthBuffer; }

	/*
	*	Returns the output render targets.
	*/
	const DynamicArray<RenderTargetHandle>& GetOutputRenderTargets() const NOEXCEPT { return _OutputRenderTargets; }

	/*
	*	Returns the vertex input attribute descriptions.
	*/
	const DynamicArray<VertexInputAttributeDescription>& GetVertexInputAttributeDescriptions() const NOEXCEPT { return _VertexInputAttributeDescriptions; }

	/*
	*	Returns the vertex input binding descriptions.
	*/
	const DynamicArray<VertexInputBindingDescription>& GetVertexInputBindingDescriptions() const NOEXCEPT { return _VertexInputBindingDescriptions; }

	/*
	*	Returns the render resolution.
	*/
	Resolution GetRenderResolution() const NOEXCEPT { return _RenderResolution; }

	/*
	*	Returns whether or not blend is enabled.
	*/
	bool IsBlendEnabled() const NOEXCEPT { return _BlendEnabled; }

	/*
	*	Returns the blend factor for the source color.
	*/
	BlendFactor GetBlendFactorSourceColor() const NOEXCEPT { return _BlendFactorSourceColor; }

	/*
	*	Returns the blend factor for the destination color.
	*/
	BlendFactor GetBlendFactorDestinationColor() const NOEXCEPT { return _BlendFactorDestinationColor; }

	/*
	*	Returns the blend factor for the source alpha.
	*/
	BlendFactor GetBlendFactorSourceAlpha() const NOEXCEPT { return _BlendFactorSourceAlpha; }

	/*
	*	Returns the blend factor for the destination alpha.
	*/
	BlendFactor GetBlendFactorDestinationAlpha() const NOEXCEPT { return _BlendFactorDestinationAlpha; }

	/*
	*	Returns the cull mode.
	*/
	CullMode GetCullMode() const NOEXCEPT { return _CullMode; }

	/*
	*	Returns the depth compare operator.
	*/
	CompareOperator GetDepthCompareOperator() const NOEXCEPT { return _DepthCompareOperator; }

	/*
	*	Returns whether or not depth test is enabled.
	*/
	bool IsDepthTestEnabled() const NOEXCEPT { return _DepthTestEnabled; }

	/*
	*	Returns whether or not depth write is enabled.
	*/
	bool IsDepthWriteEnabled() const NOEXCEPT { return _DepthWriteEnabled; }

	/*
	*	Returns whether or not stencil test is enabled.
	*/
	bool IsStencilTestEnabled() const NOEXCEPT { return _StencilTestEnabled; }

	/*
	*	Returns the action performed on samples that fail the stencil test.
	*/
	StencilOperator GetStencilFailOperator() const NOEXCEPT { return _StencilFailOperator; }

	/*
	*	Returns the action performed on samples that pass both the depth test and the stencil test.
	*/
	StencilOperator GetStencilPassOperator() const NOEXCEPT { return _StencilPassOperator; }

	/*
	*	Returns the action performed on samples that fail the depth test but pass the stencil test.
	*/
	StencilOperator GetStencilDepthFailOperator() const NOEXCEPT { return _StencilDepthFailOperator; }

	/*
	*	Returns the stencil compare operator.
	*/
	CompareOperator GetStencilCompareOperator() const NOEXCEPT { return _StencilCompareOperator; }

	/*
	*	Returns the stencil compare mask.
	*/
	uint32 GetStencilCompareMask() const NOEXCEPT { return _StencilCompareMask; }

	/*
	*	Returns the stencil write mask.
	*/
	uint32 GetStencilWriteMask() const NOEXCEPT { return _StencilWriteMask; }

	/*
	*	Returns the stencil reference mask.
	*/
	uint32 GetStencilReferenceMask() const NOEXCEPT { return _StencilCompareMask; }

	/*
	*	Returns the depth/stencil attachment load operator.
	*/
	FORCE_INLINE AttachmentLoadOperator GetDepthStencilAttachmentLoadOperator() const NOEXCEPT
	{
		return _DepthStencilAttachmentLoadOperator;
	}

	/*
	*	Returns the depth/stencil attachment store operator.
	*/
	FORCE_INLINE AttachmentStoreOperator GetDepthStencilAttachmentStoreOperator() const NOEXCEPT
	{
		return _DepthStencilAttachmentStoreOperator;
	}

	/*
	*	Returns the color attachment load operator.
	*/
	FORCE_INLINE AttachmentLoadOperator GetColorAttachmentLoadOperator() const NOEXCEPT
	{
		return _ColorAttachmentLoadOperator;
	}

	/*
	*	Returns the color attachment store operator.
	*/
	FORCE_INLINE AttachmentStoreOperator GetColorAttachmentStoreOperator() const NOEXCEPT
	{
		return _ColorAttachmentStoreOperator;
	}

	/*
	*	Returns the topology.
	*/
	Topology GetTopology() const NOEXCEPT { return _Topology; }

	/*
	*	Sets the vertex shader.
	*/
	void SetVertexShader(const ResourcePointer<ShaderResource> newVertexShader) NOEXCEPT { _VertexShader = newVertexShader; }

	/*
	*	Sets the tessellation control shader.
	*/
	void SetTessellationControlShader(const ResourcePointer<ShaderResource> newTessellationControlShader) NOEXCEPT { _TessellationControlShader = newTessellationControlShader; }

	/*
	*	Sets the tessellation evaluation shader.
	*/
	void SetTessellationEvaluationShader(const ResourcePointer<ShaderResource> newTessellationEvaluationShader) NOEXCEPT { _TessellationEvaluationShader = newTessellationEvaluationShader; }

	/*
	*	Sets the geometry shader.
	*/
	void SetGeometryShader(const ResourcePointer<ShaderResource> newGeometryShader) NOEXCEPT { _GeometryShader = newGeometryShader; }

	/*
	*	Sets the fragment shader.
	*/
	void SetFragmentShader(const ResourcePointer<ShaderResource> newFragmentShader) NOEXCEPT { _FragmentShader = newFragmentShader; }

	/*
	*	Sets the depth buffer.
	*/
	void SetDepthBuffer(const DepthBufferHandle depthBuffer) NOEXCEPT { _DepthBuffer = depthBuffer; }

	/*
	*	Sets the number of output render targets.
	*/
	void SetNumberOfOutputRenderTargets(const uint64 value) NOEXCEPT { _OutputRenderTargets.Reserve(value); }

	/*
	*	Adds an outputrender target.
	*/
	void AddOutputRenderTarget(const RenderTargetHandle value) NOEXCEPT { _OutputRenderTargets.Emplace(value); }

	/*
	*	Sets the number of vertex input attribute descriptions.
	*/
	void SetNumberOfVertexInputAttributeDescriptions(const uint64 numberOfVertexInputAttributeDescriptions) NOEXCEPT { _VertexInputAttributeDescriptions.Reserve(numberOfVertexInputAttributeDescriptions); }

	/*
	*	Adds a vertex input attribute description.
	*/
	void AddVertexInputAttributeDescription(const uint32 location, const uint32 binding, const VertexInputAttributeDescription::Format format, const uint32 offset) NOEXCEPT { _VertexInputAttributeDescriptions.Emplace(location, binding, format, offset); }

	/*
	*	Sets the number of vertex input binding descriptions.
	*/
	void SetNumberOfVertexInputBindingDescriptions(const uint64 numberOfVertexInputBindingDescriptions) NOEXCEPT { _VertexInputBindingDescriptions.Reserve(numberOfVertexInputBindingDescriptions); }

	/*
	*	Adds a vertex input binding description.
	*/
	void AddVertexInputBindingDescription(const uint32 binding, const uint32 stride, const VertexInputBindingDescription::InputRate inputRate) NOEXCEPT { _VertexInputBindingDescriptions.Emplace(binding, stride, inputRate); }

	/*
	*	Sets the render resolution.
	*/
	void SetRenderResolution(const Resolution &newRenderResolution) NOEXCEPT { _RenderResolution = newRenderResolution; }

	/*
	*	Sets the depth/stencil attachment load operator.
	*/
	FORCE_INLINE void SetDepthStencilAttachmentLoadOperator(const AttachmentLoadOperator value) NOEXCEPT
	{
		_DepthStencilAttachmentLoadOperator = value;
	}

	/*
	*	Sets the depth/stencil attachment store operator.
	*/
	FORCE_INLINE void SetDepthStencilAttachmentStoreOperator(const AttachmentStoreOperator value) NOEXCEPT
	{
		_DepthStencilAttachmentStoreOperator = value;
	}

	/*
	*	Sets the color attachment load operator.
	*/
	FORCE_INLINE void SetColorAttachmentLoadOperator(const AttachmentLoadOperator value) NOEXCEPT
	{
		_ColorAttachmentLoadOperator = value;
	}

	/*
	*	Sets the color attachment store operator.
	*/
	FORCE_INLINE void SetColorAttachmentStoreOperator(const AttachmentStoreOperator value) NOEXCEPT
	{
		_ColorAttachmentStoreOperator = value;
	}

	/*
	*	Sets whether or not blend is enabled.
	*/
	void SetBlendEnabled(const bool newBlendEnabled) NOEXCEPT { _BlendEnabled = newBlendEnabled; }

	/*
	*	Sets the blend factor for the source color.
	*/
	void SetBlendFactorSourceColor(const BlendFactor newBlendFactor) NOEXCEPT { _BlendFactorSourceColor = newBlendFactor; }

	/*
	*	Sets the blend factor for the destination color.
	*/
	void SetBlendFactorDestinationColor(const BlendFactor newBlendFactor) NOEXCEPT { _BlendFactorDestinationColor = newBlendFactor; }

	/*
	*	Sets the blend factor for the source alpha.
	*/
	void SetBlendFactorSourceAlpha(const BlendFactor newBlendFactor) NOEXCEPT { _BlendFactorSourceAlpha = newBlendFactor; }

	/*
	*	Sets the blend factor for the destination alpha.
	*/
	void SetBlendFactorDestinationAlpha(const BlendFactor newBlendFactor) NOEXCEPT { _BlendFactorDestinationAlpha = newBlendFactor; }

	/*
	*	Sets the cull mode.
	*/
	void SetCullMode(const CullMode newCullMode) NOEXCEPT { _CullMode = newCullMode; }

	/*
	*	Sets the depth compare operator.
	*/
	void SetDepthCompareOperator(const CompareOperator newDepthCompareOperator) NOEXCEPT { _DepthCompareOperator = newDepthCompareOperator; }

	/*
	*	Sets whether or not depth test is enabled.
	*/
	void SetDepthTestEnabled(const bool newDepthTestEnabled) NOEXCEPT { _DepthTestEnabled = newDepthTestEnabled; }

	/*
	*	Sets whether or not depth write is enabled.
	*/
	void SetDepthWriteEnabled(const bool newDepthWriteEnabled) NOEXCEPT { _DepthWriteEnabled = newDepthWriteEnabled; }

	/*
	*	Sets whether or not stencil test is enabled.
	*/
	void SetStencilTestEnabled(const bool newStencilTestEnabled) NOEXCEPT { _StencilTestEnabled = newStencilTestEnabled; }

	/*
	*	Sets the action performed on samples that fail the stencil test.
	*/
	void SetStencilFailOperator(const StencilOperator newStencilFailOperator) NOEXCEPT { _StencilFailOperator = newStencilFailOperator; }

	/*
	*	Sets the action performed on samples that pass both the depth test and the stencil test.
	*/
	void SetStencilPassOperator(const StencilOperator newStencilPassOperator) NOEXCEPT { _StencilPassOperator = newStencilPassOperator; }

	/*
	*	Sets the action performed on samples that fail the depth test but pass the stencil test.
	*/
	void SetStencilDepthFailOperator(const StencilOperator newStencilDepthFailOperator) NOEXCEPT { _StencilDepthFailOperator = newStencilDepthFailOperator; }

	/*
	*	Sets the stencil compare operator.
	*/
	void SetStencilCompareOperator(const CompareOperator newStencilCompareOperator) NOEXCEPT { _StencilCompareOperator = newStencilCompareOperator; }

	/*
	*	Sets the stencil compare mask.
	*/
	void SetStencilCompareMask(const uint32 newStencilCompareMask) NOEXCEPT { _StencilCompareMask = newStencilCompareMask; }

	/*
	*	Sets the stencil write mask.
	*/
	void SetStencilWriteMask(const uint32 newStencilWriteMask) NOEXCEPT { _StencilWriteMask = newStencilWriteMask; }

	/*
	*	Sets the stencil reference mask.
	*/
	void SetStencilReferenceMask(const uint32 newStencilReferenceMask) NOEXCEPT { _StencilCompareMask = newStencilReferenceMask; }

	/*
	*	Sets the topology.
	*/
	void SetTopology(const Topology newTopology) NOEXCEPT { _Topology = newTopology; }

private:

	//The vertex shader.
	ResourcePointer<ShaderResource> _VertexShader;

	//The tessellation control shader.
	ResourcePointer<ShaderResource> _TessellationControlShader;

	//The tessellation evaluation shader.
	ResourcePointer<ShaderResource> _TessellationEvaluationShader;

	//The geometry shader.
	ResourcePointer<ShaderResource> _GeometryShader;

	//The fragment shader.
	ResourcePointer<ShaderResource> _FragmentShader;

	//The depth buffer.
	DepthBufferHandle _DepthBuffer{ EMPTY_HANDLE };

	//The output render targets.
	DynamicArray<RenderTargetHandle> _OutputRenderTargets;

	//The vertex input attribute descriptions.
	DynamicArray<VertexInputAttributeDescription> _VertexInputAttributeDescriptions;

	//The vertex input binding descriptions.
	DynamicArray<VertexInputBindingDescription> _VertexInputBindingDescriptions;

	//The render resolution.
	Resolution _RenderResolution;

	//The depth/stencil attachment load operator.
	AttachmentLoadOperator _DepthStencilAttachmentLoadOperator;

	//The depth/stencil attachment store operator.
	AttachmentStoreOperator _DepthStencilAttachmentStoreOperator;

	//The color attachment load operator.
	AttachmentLoadOperator _ColorAttachmentLoadOperator;

	//The color attachment store operator.
	AttachmentStoreOperator _ColorAttachmentStoreOperator;

	//Denotes whether or not blend is enabled.
	bool _BlendEnabled;

	//The blend factor for the source color.
	BlendFactor _BlendFactorSourceColor;

	//The blend factor for the destination color.
	BlendFactor _BlendFactorDestinationColor;

	//The blend factor for the source alpha.
	BlendFactor _BlendFactorSourceAlpha;

	//The blend factor for the destination alpha.
	BlendFactor _BlendFactorDestinationAlpha;

	//The cull mode.
	CullMode _CullMode;

	//The depth compare operator.
	CompareOperator _DepthCompareOperator;

	//Denotes whether or not depth test is enabled.
	bool _DepthTestEnabled;

	//Denotes whether or not depth write is enabled.
	bool _DepthWriteEnabled;

	//Denotes whether or not stencil test is enabled.
	bool _StencilTestEnabled;

	//The action performed on samples that fail the stencil test.
	StencilOperator _StencilFailOperator;

	//The action performed on samples that pass both the depth test and the stencil test.
	StencilOperator _StencilPassOperator;

	//The action performed on samples that fail the depth test but pass the stencil test.
	StencilOperator _StencilDepthFailOperator;

	//The stencil compare operator.
	CompareOperator _StencilCompareOperator;

	//The stencil compare mask.
	uint32 _StencilCompareMask;

	//The stencil write mask.
	uint32 _StencilWriteMask;

	//The stencil reference mask.
	uint32 _StencilReferenceMask;

	//The topology.
	Topology _Topology;

};