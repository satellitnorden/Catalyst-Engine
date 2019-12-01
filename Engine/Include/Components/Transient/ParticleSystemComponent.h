#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/ParticleInstanceData.h>

class ParticleSystemComponent final
{

public:

	//The position.
	Vector3<float> _Position;

	//The minimum position.
	Vector3<float> _MinimumPosition;

	//The maximum position.
	Vector3<float> _MaximumPosition;

	//The minimum velocity.
	Vector3<float> _MinimumVelocity;

	//The maximum velocity.
	Vector3<float> _MaximumVelocity;

	//The minimum scale.
	Vector2<float> _MinimumScale;

	//The maximum scale.
	Vector2<float> _MaximumScale;

	//The number of instances.
	uint32 _NumberOfInstances;

	//The spawn frequency.
	float _SpawnFrequency;

	//The lifetime.
	float _Lifetime;

	//The fade time.
	float _FadeTime;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The time since the last instance spawn.
	float _TimeSinceLastSpawn;

	//The first particle index to spawn this frame.
	uint32 _FirstParticleIndexToSpawn;

	//The number of particles to spawn this frame.
	uint32 _NumberOfParticlesToSpawn;

};