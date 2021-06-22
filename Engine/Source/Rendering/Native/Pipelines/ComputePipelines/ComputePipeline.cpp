//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

/*
*	Default constructor.
*/
ComputePipeline::ComputePipeline() NOEXCEPT
{
	//Set the type.
	SetType(Type::Compute);
}

/*
*	Resets this compute pipeline.
*/
void ComputePipeline::ResetComputePipeline() NOEXCEPT
{
	//Reset this pipeline.
	ResetPipeline();
}