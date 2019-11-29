//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/ParticleSystemComputePipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes this compute pipeline.
*/
void ParticleSystemComputePipeline::Initialize() NOEXCEPT
{
	//Set the shader.
	SetShader(Shader::ParticleSystemCompute);
}

/*
*	Executes this compute pipeline.
*/
void ParticleSystemComputePipeline::Execute() NOEXCEPT
{
	//Iterate over all particle system components and update them all.
	const uint64 number_of_components{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there's none to update - update none.
	if (number_of_components == 0 || true)
	{
		//Don't include this pipeline in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer* const RESTRICT commandBuffer{ GetCurrentCommandBuffer() };
	const ParticleSystemComponent* RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };

	//Begin the command buffer.
	commandBuffer->Begin(this);

	//Bind the render data tables.
	commandBuffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{

	}

	//End the command buffer.
	commandBuffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}