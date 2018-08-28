#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/General/Padding.h>

//Rendering.
#include <Rendering/Engine/ParticleSystemProperties.h>

class VulkanParticleSystemProperties final
{

public:

	//The fade time.
	float fadeTime;

	//The lifetime of the particles.
	float lifetime;

	//The spawn frequency.
	float spawnFrequency;

	//Some padding.
	Padding<4> padding1;

	//The minimum scale.
	Vector2 minimumScale;

	//The maximum scale.
	Vector2 maximumScale;

	//The minimum position.
	Vector3 minimumPosition;

	//Some padding.
	Padding<1> padding2;

	//The maximum position.
	Vector3 maximumPosition;

	//Some padding.
	Padding<1> padding3;

	//The minimum velocity.
	Vector3 minimumVelocity;

	//Some padding.
	Padding<1> padding4;

	//The maximum velocity.
	Vector3 maximumVelocity;

	//Some padding.
	Padding<1> padding5;

	//The world position.
	Vector3 worldPosition;

	//Some padding.
	Padding<1> padding6;

	/*
	*	Constructor taking the regular particle system properties.
	*/
	VulkanParticleSystemProperties(const ParticleSystemProperties &otherProperties) NOEXCEPT
		:
		fadeTime(otherProperties.fadeTime),
		lifetime(otherProperties.lifetime),
		spawnFrequency(otherProperties.spawnFrequency),
		minimumScale(otherProperties.minimumScale),
		maximumScale(otherProperties.maximumScale),
		minimumPosition(otherProperties.minimumPosition),
		maximumPosition(otherProperties.maximumPosition),
		minimumVelocity(otherProperties.minimumVelocity),
		maximumVelocity(otherProperties.maximumVelocity),
		worldPosition(otherProperties.worldPosition)
	{

	}

};

static_assert(offsetof(VulkanParticleSystemProperties, fadeTime) == 0, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, lifetime) == 4, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, spawnFrequency) == 8, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, minimumScale) == 16, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, maximumScale) == 24, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, minimumPosition) == 32, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, maximumPosition) == 48, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, minimumVelocity) == 64, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, maximumVelocity) == 80, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, worldPosition) == 96, "Member of VulkanParticleSystemProperties is not layout correctly.");

static_assert(sizeof(VulkanParticleSystemProperties) == 112, "VulkanParticleSystemProperties has incorrect size.");