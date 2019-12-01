//Header file.
#include <Rendering/Native/Pipelines/ComputePipelines/ParticleSystemComputePipeline.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Particle system compute push constant data definition.
*/
class ParticleSystemComputePushConstantData final
{

public:

	Vector3<float> _Position;
	Vector3<float> _MinimumPosition;
	Vector3<float> _MaximumPosition;
	Vector3<float> _MinimumVelocity;
	Vector3<float> _MaximumVelocity;
	Vector2<float> _MinimumSize;
	Vector2<float> _MaximumSize;
	float _Lifetime;
	float _FadeTime;
	float _PositionSeed1;
	float _PositionSeed2;
	float _PositionSeed3;
	float _VelocitySeed1;
	float _VelocitySeed2;
	float _VelocitySeed3;
	float _SizeSeed;
	uint32 _FirstParticleIndexToSpawn;
	uint32 _NumberOfParticlesToSpawn;
	uint32 _NumberOfInstances;

};

/*
*	Initializes this compute pipeline.
*/
void ParticleSystemComputePipeline::Initialize() NOEXCEPT
{
	//Set the shader.
	SetShader(Shader::ParticleSystemCompute);

	//Add the render data table layouts.
	SetNumberOfRenderDataTableLayouts(2);
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Global));
	AddRenderDataTableLayout(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem));

	//Add the push constant ranges.
	SetNumberOfPushConstantRanges(1);
	AddPushConstantRange(ShaderStage::Compute, 0, sizeof(ParticleSystemComputePushConstantData));
}

/*
*	Executes this compute pipeline.
*/
void ParticleSystemComputePipeline::Execute() NOEXCEPT
{
	//Iterate over all particle system components and update them all.
	const uint64 number_of_components{ ComponentManager::GetNumberOfParticleSystemComponents() };

	//If there's none to update - update none.
	if (number_of_components == 0)
	{
		//Don't include this pipeline in the final render.
		SetIncludeInRender(false);

		return;
	}

	//Cache data the will be used.
	CommandBuffer* const RESTRICT command_buffer{ GetCurrentCommandBuffer() };
	const ParticleSystemComponent* RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };

	//Begin the command buffer.
	command_buffer->Begin(this);

	//Bind the render data tables.
	command_buffer->BindRenderDataTable(this, 0, RenderingSystem::Instance->GetGlobalRenderDataTable());

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		//Push constants.
		ParticleSystemComputePushConstantData data;

		data._Position = component->_Position;
		data._MinimumPosition = component->_MinimumPosition;
		data._MaximumPosition = component->_MaximumPosition;
		data._MinimumVelocity = component->_MinimumVelocity;
		data._MaximumVelocity = component->_MaximumVelocity;
		data._MinimumSize = component->_MinimumScale;
		data._MaximumSize = component->_MaximumScale;
		data._Lifetime = component->_Lifetime;
		data._FadeTime = component->_FadeTime;
		data._PositionSeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._PositionSeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._PositionSeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._VelocitySeed1 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._VelocitySeed2 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._VelocitySeed3 = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._SizeSeed = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
		data._FirstParticleIndexToSpawn = component->_FirstParticleIndexToSpawn;
		data._NumberOfParticlesToSpawn = component->_NumberOfParticlesToSpawn;
		data._NumberOfInstances = component->_NumberOfInstances;

		command_buffer->PushConstants(this, ShaderStage::Compute, 0, sizeof(ParticleSystemComputePushConstantData), &data);

		//Bind the render data table.
		command_buffer->BindRenderDataTable(this, 1, component->_RenderDataTable);

		//Dispatch!
		command_buffer->Dispatch(this, component->_NumberOfInstances, 1, 1);
	}

	//End the command buffer.
	command_buffer->End(this);

	//Include this render pass in the final render.
	SetIncludeInRender(true);
}