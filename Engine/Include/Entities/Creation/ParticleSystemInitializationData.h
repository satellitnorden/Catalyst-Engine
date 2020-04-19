#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>
#include <Entities/Types/ParticleSystemEntity.h>

//Math.
#include <Math/General/Vector.h>

class ParticleSystemInitializationData final : public EntityInitializationData
{

public:

	//The particle system properties.
	ParticleSystemEntity::Property _ParticleSystemProperties;

	//The initial position.
	Vector3<float> _InitialPosition;

	//The material index.
	uint32 _MaterialIndex;

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

	//The initial spawn frequency.
	float _InitialSpawnFrequency;

	//The lifetime.
	float _Lifetime;

	//The fade time.
	float _FadeTime;

};