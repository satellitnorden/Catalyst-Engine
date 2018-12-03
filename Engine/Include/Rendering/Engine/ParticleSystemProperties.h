#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

	//Enumeration covering all different particle system properties.
enum class ParticleSystemProperty : int32
{
	AffectedByWind = BIT(0),
	Looping = BIT(1)
};

ENUMERATION_BIT_OPERATIONS(ParticleSystemProperty);

class ParticleSystemProperties final
{

public:

	//The properties.
	ParticleSystemProperty _Properties;

	//The fade time.
	float _FadeTime;

	//The lifetime of the particles.
	float _Lifetime;

	//The spawn frequency.
	float _SpawnFrequency;

	//The minimum scale.
	Vector2<float> _MinimumScale;

	//The maximum scale.
	Vector2<float> _MaximumScale;

	//The minimum position.
	Vector3 _MinimumPosition;

	//Some padding.
	Padding<1> _Padding1;

	//The maximum position.
	Vector3 _MaximumPosition;

	//Some padding.
	Padding<1> _Padding2;

	//The minimum velocity.
	Vector3 _MinimumVelocity;

	//Some padding.
	Padding<1> _Padding3;

	//The maximum velocity.
	Vector3 _MaximumVelocity;

	//Some padding.
	Padding<1> _Padding4;

};