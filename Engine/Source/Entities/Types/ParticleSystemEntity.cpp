//Header file.
#include <Entities/Types/ParticleSystemEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Creation/ParticleSystemInitializationData.h>

//Math.
#include <Math/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntityCreationSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
ParticleSystemEntity::ParticleSystemEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::ParticleSystem;
}

/*
*	Initializes this entity.
*/
void ParticleSystemEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this particle system entity.
	_ComponentsIndex = ComponentManager::GetNewParticleSystemComponentsIndex(this);

	//Cache the data.
	ParticleSystemComponent &component{ ComponentManager::GetParticleSystemParticleSystemComponents()[_ComponentsIndex] };
	ParticleSystemRenderComponent &renderComponent{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex] };
	const ParticleSystemInitializationData *const RESTRICT particleSystemInitializationData{ static_cast<const ParticleSystemInitializationData *const RESTRICT>(data) };

	//Fill in the components.
	RenderingUtilities::CalculateAxisAlignedBoundingBoxForParticleSystem(particleSystemInitializationData->_ParticleSystemProperties._WorldPosition, particleSystemInitializationData->_ParticleSystemProperties, &component._AxisAlignedBoundingBox);
	renderComponent._Properties = particleSystemInitializationData->_ParticleSystemProperties;
	RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::ParticleSystem), &renderComponent._RenderDataTable);
	RenderingSystem::Instance->BindCombinedImageSamplerToRenderDataTable(0, 0, renderComponent._RenderDataTable, particleSystemInitializationData->_Material._AlbedoTexture, RenderingSystem::Instance->GetSampler(Sampler::FilterLinear_MipmapModeLinear_AddressModeClampToEdge));
	renderComponent._InstanceCount = CatalystBaseMath::Round<uint32>(particleSystemInitializationData->_ParticleSystemProperties._Lifetime / particleSystemInitializationData->_ParticleSystemProperties._SpawnFrequency);
	renderComponent._ParticleSystemRandomSeed = CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f);
	renderComponent._ParticleSystemStartingTime = CatalystEngineSystem::Instance->GetTotalTime();

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<ParticleSystemInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void ParticleSystemEntity::Terminate() NOEXCEPT
{
	//Destroy the render data table.
	RenderingSystem::Instance->DestroyRenderDataTable(ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex]._RenderDataTable);

	//Return this entitiy's components index.
	ComponentManager::ReturnParticleSystemComponentsIndex(_ComponentsIndex);
}

/*
*	Returns whether or not this entity should automatically terminate.
*/
bool ParticleSystemEntity::ShouldAutomaticallyTerminate() const NOEXCEPT
{
	//Return whether or not this entity should automatically terminate.
	const ParticleSystemProperties &properties{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex]._Properties };
	const bool isLooping{ TEST_BIT(properties._Properties, ParticleSystemProperty::Looping) };

	if (isLooping)
	{
		return false;
	}

	else
	{
		const float startingTime{ ComponentManager::GetParticleSystemParticleSystemRenderComponents()[_ComponentsIndex]._ParticleSystemStartingTime };
		const float totalTime{ CatalystEngineSystem::Instance->GetTotalTime() };
		const float elapsedTime{ totalTime - startingTime };

		return elapsedTime >= properties._Lifetime - properties._SpawnFrequency + properties._Lifetime;
	}
}