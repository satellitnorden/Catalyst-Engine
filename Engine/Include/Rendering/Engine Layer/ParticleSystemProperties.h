#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

class ParticleSystemProperties final
{

public:

	//The fade time, for fade-ins/fade-outs.
	float fadeTime;

	//The lifetime of the particles.
	float lifetime;

	//The spawn frequency.
	float spawnFrequency;

	//The minimum scale.
	Vector2 minimumScale;

	//The maximum scale.
	Vector2 maximumScale;

	//The minimum position.
	Vector3 minimumPosition;

	//The maximum position.
	Vector3 maximumPosition;

	//The minimum velocity.
	Vector3 minimumVelocity;

	//The maximum velocity.
	Vector3 maximumVelocity;

	//The world position.
	Vector3 worldPosition;

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
		fadeTime(initialFadeTime),
		lifetime(initialLifetime),
		spawnFrequency(initialSpawnFrequency),
		minimumScale(initialMinimumScale),
		maximumScale(initialMaximumScale),
		minimumPosition(initialMinimumPosition),
		maximumPosition(initialMaximumPosition),
		minimumVelocity(initialMinimumVelocity),
		maximumVelocity(initialMaximumVelocity),
		worldPosition(initialWorldPosition)
	{

	}

};