#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//World.
#include <World/Core/WorldPosition.h>

/*
*	Particle instance class definition.
*/
class ParticleInstance final
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The size.
	Vector2<float32> _Size;

	//The age.
	float32 _Age;

	//The lifetime.
	float32 _Lifetime;

};

/*
*	Particle packed instance class definition.
*/
class ParticlePackedInstance final
{

public:

	//The world position.
	Vector3<float32> _WorldPosition;

	//The size.
	Vector2<float32> _Size;

	//The normalized age.
	float32 _NormalizedAge;

};

/*
*	Particle system shared data class definition.
*/
class ParticleSystemSharedData final
{

};

/*
*	Particle system initialization data class definition.
*/
class ParticleSystemInitializationData final : public ComponentInitializationData
{

public:


};

/*
*	Particle system instance data class definition.
*/
class ParticleSystemInstanceData final
{

public:

	//The instances.
	DynamicArray<ParticleInstance> _Instances;

	//The packed instancs.
	DynamicArray<ParticlePackedInstance> _PackedInstances;

};

DECLARE_COMPONENT(ParticleSystemComponent, ParticleSystemSharedData, ParticleSystemInitializationData, ParticleSystemInstanceData);