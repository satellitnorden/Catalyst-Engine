#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class ParticleSystemComputePipeline final : public ComputePipeline
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

	/*
	*	Terminates this compute pipeline.
	*/
	void Terminate() NOEXCEPT;

};