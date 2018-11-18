#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class TerrainDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TerrainDepthRenderPass);

	/*
	*	Default constructor.
	*/
	TerrainDepthRenderPass() NOEXCEPT;

private:

	//The instance buffers.
	DynamicArray<UniformBufferHandle> _InstanceBuffers;

	/*
	*	Initializes the terrain depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Initializes the instance buffers.
	*/
	void InitializeInstanceBuffers() NOEXCEPT;

	/*
	*	Renders the depth of the terrain.
	*/
	void RenderInternal() NOEXCEPT;

};