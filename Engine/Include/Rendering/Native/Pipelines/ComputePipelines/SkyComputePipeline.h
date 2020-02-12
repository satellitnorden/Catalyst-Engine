#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class SkyComputePipeline final : public ComputePipeline
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

	//Keeps track of the current iteration.
	uint32 _CurrentIteration{ 0 };

	/*
	*	Calculates the parameters difference.
	*/
	NO_DISCARD float CalculateParametersDifference() NOEXCEPT;

};