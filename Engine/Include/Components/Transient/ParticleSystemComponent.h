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

	//The global material index.
	uint32 _GlobalMaterialIndex;

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

	//The spawn frequency.
	float _SpawnFrequency;

	//The lifetime.
	float _Lifetime;

	//The fade time.
	float _FadeTime;

	//The instance data.
	DynamicArray<ParticleInstanceData> _InstanceData;

	//The time since the last instance spawn.
	float _TimeSinceLastSpawn;

};