#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Types/ParticleSystemEntity.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/ParticleInstanceData.h>

//World.
#include <World/Core/WorldPosition.h>

class ParticleSystemComponent final
{

public:

	//The particle system properties.
	ParticleSystemEntity::Property _ParticleSystemProperties;

	//The original world position.
	WorldPosition _OriginalWorldPosition;

	//The current world position.
	WorldPosition _CurrentWorldPosition;

	//The minimum position.
	Vector3<float32> _MinimumPosition;

	//The maximum position.
	Vector3<float32> _MaximumPosition;

	//The minimum velocity.
	Vector3<float32> _MinimumVelocity;

	//The maximum velocity.
	Vector3<float32> _MaximumVelocity;

	//The minimum scale.
	Vector2<float32> _MinimumScale;

	//The maximum scale.
	Vector2<float32> _MaximumScale;

	//The number of instances.
	uint32 _NumberOfInstances;

	//The spawn frequency, defined in particles per seconds.
	float32 _SpawnFrequency;

	//The lifetime.
	float32 _Lifetime;

	//The fade time.
	float32 _FadeTime;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The time since the last instance spawn.
	float32 _TimeSinceLastSpawn;

	//The first particle index to spawn this frame.
	uint32 _FirstParticleIndexToSpawn;

	//The number of particles to spawn this frame.
	uint32 _NumberOfParticlesToSpawn;

};