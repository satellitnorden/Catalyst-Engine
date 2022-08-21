#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class MobileGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const DepthBufferHandle depth_buffer) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this graphics pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//Enumeration covering all mobile passes.
	enum class MobilePass : uint8
	{
		MODEL,
		SKY
	};

	//The sky vertex buffer.
	BufferHandle _SkyVertexBuffer;

	//The sky index buffer.
	BufferHandle _SkyIndexBuffer;

};