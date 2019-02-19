#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

	//Enumeration covering all different particle system properties.
enum class ParticleSystemProperty : uint8
{
	AffectedByWind = BIT(0),
	Looping = BIT(1)
};

ENUMERATION_BIT_OPERATIONS(ParticleSystemProperty);

class ParticleSystemProperties final
{

public:

	//The minimum position.
	Vector3<float> _MinimumPosition;

	//The maximum position.
	Vector3<float> _MaximumPosition;

	//The minimum velocity.
	Vector3<float> _MinimumVelocity;

	//The maximum velocity.
	Vector3<float> _MaximumVelocity;

	//The world position.
	Vector3<float> _WorldPosition;

	//The minimum scale.
	Vector2<float> _MinimumScale;

	//The maximum scale.
	Vector2<float> _MaximumScale;

	//The fade time.
	float _FadeTime;

	//The lifetime of the particles.
	float _Lifetime;

	//The spawn frequency.
	float _SpawnFrequency;

	//The properties.
	ParticleSystemProperty _Properties;

};