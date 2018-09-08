#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

class ParticleSystemProperties final
{

public:

	//The fade time, for fade-ins/fade-outs.
	float _FadeTime;

	//The lifetime of the particles.
	float _Lifetime;

	//The spawn frequency.
	float _SpawnFrequency;

	//The minimum scale.
	Vector2 _MinimumScale;

	//The maximum scale.
	Vector2 _MaximumScale;

	//The minimum position.
	Vector3 _MinimumPosition;

	//The maximum position.
	Vector3 _MaximumPosition;

	//The minimum velocity.
	Vector3 _MinimumVelocity;

	//The maximum velocity.
	Vector3 _MaximumVelocity;

	//The world position.
	Vector3 _WorldPosition;

	/*
	*	Default constructor.
	*/
	ParticleSystemProperties() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all properties as arguments.
	*/
	ParticleSystemProperties(	const float initialFadeTime,
								const float initialLifetime,
								const float initialSpawnFrequency,
								const Vector2 &initialMinimumScale,
								const Vector2 &initialMaximumScale,
								const Vector3 &initialMinimumPosition,
								const Vector3 &initialMaximumPosition,
								const Vector3 &initialMinimumVelocity,
								const Vector3 &initialMaximumVelocity,
								const Vector3 &initialWorldPosition) NOEXCEPT
		:
		_FadeTime(initialFadeTime),
		_Lifetime(initialLifetime),
		_SpawnFrequency(initialSpawnFrequency),
		_MinimumScale(initialMinimumScale),
		_MaximumScale(initialMaximumScale),
		_MinimumPosition(initialMinimumPosition),
		_MaximumPosition(initialMaximumPosition),
		_MinimumVelocity(initialMinimumVelocity),
		_MaximumVelocity(initialMaximumVelocity),
		_WorldPosition(initialWorldPosition)
	{

	}

};