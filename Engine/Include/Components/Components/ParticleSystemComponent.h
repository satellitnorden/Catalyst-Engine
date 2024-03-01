#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

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

	//The velocity.
	Vector3<float32> _Velocity;

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

	//Padding.
	Padding<8> _Padding;

};

/*
*	Particle emitter class definition.
*/
class ParticleEmitter final
{

public:

	//Enumeration covering all emit modes.
	enum class EmitMode : uint8
	{
		/*
		*	Picks random positions in a sphere of '_SphereMode._Radius' radius.
		*/
		SPHERE
	};

	//The emit mode.
	EmitMode _EmitMode;

	union
	{
		struct
		{
			//The radius.
			float32 _Radius;
		} _SphereMode;
	};

	//The minimum velocity.
	Vector3<float32> _MinimumVelocity;

	//The maximum velocity.
	Vector3<float32> _MaximumVelocity;

	//The minimum size.
	Vector2<float32> _MinimumSize;

	//The maximum size.
	Vector2<float32> _MaximumSize;

	//The minimum lifetime.
	float32 _MinimumLifetime;

	//The maximum lifetime.
	float32 _MaximumLifetime;

	//The spawn rate, expressed in instances per second.
	uint32 _SpawnRate;

};

/*
*	Particle sub emitter class definition.
*/
class ParticleSubEmitter final
{

public:

	//The emitter.
	ParticleEmitter _Emitter;

	//The time since the last particle spawn.
	float32 _TimeSinceLastParticleSpawn;

	//The instances.
	DynamicArray<ParticleInstance> _Instances;

	//The packed instancs.
	DynamicArray<ParticlePackedInstance> _PackedInstances;

};

/*
*	Particle system shared data class definition.
*/
class ParticleSystemSharedData final
{

public:

	//The packed instancs.
	DynamicArray<ParticlePackedInstance> _PackedInstances;

};

/*
*	Particle system initialization data class definition.
*/
class ParticleSystemInitializationData final : public ComponentInitializationData
{

public:

	//The emitter.
	ParticleEmitter _Emitter;

};

/*
*	Particle system instance data class definition.
*/
class ParticleSystemInstanceData final
{

public:

	//The sub emitters.
	DynamicArray<ParticleSubEmitter> _SubEmitters;

	//The start instance index.
	uint32 _StartInstanceIndex;

	//The number of instances.
	uint32 _NumberOfInstances;

};

DECLARE_COMPONENT
(
	ParticleSystemComponent,
	ParticleSystemInitializationData,
	ParticleSystemInstanceData,
public:
	FORCE_INLINE NO_DISCARD ParticleSystemSharedData &SharedData() NOEXCEPT
	{
		return _SharedData;
	}
private:
	ParticleSystemSharedData _SharedData;
);