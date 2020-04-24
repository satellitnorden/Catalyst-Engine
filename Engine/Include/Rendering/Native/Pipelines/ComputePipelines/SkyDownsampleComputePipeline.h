#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class SkyDownsampleComputePipeline final : public ComputePipeline
{

public:

	/*
	*	Initializes this compute pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this compute pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The current face index.
	uint32 _CurrentFaceIndex{ 0 };

};