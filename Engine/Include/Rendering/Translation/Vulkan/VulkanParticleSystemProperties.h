#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Padding.h>

//Rendering.
#include <Rendering/Engine/ParticleSystemProperties.h>

class VulkanParticleSystemProperties final
{

public:

	//The fade time.
	float _FadeTime;

	//The lifetime of the particles.
	float _Lifetime;

	//The spawn frequency.
	float _SpawnFrequency;

	//Some padding.
	Padding<4> _Padding1;

	//The minimum scale.
	Vector2 _MinimumScale;

	//The maximum scale.
	Vector2 _MaximumScale;

	//The minimum position.
	Vector3 _MinimumPosition;

	//Some padding.
	Padding<1> _Padding2;

	//The maximum position.
	Vector3 _MaximumPosition;

	//Some padding.
	Padding<1> _Padding3;

	//The minimum velocity.
	Vector3 _MinimumVelocity;

	//Some padding.
	Padding<1> _Padding4;

	//The maximum velocity.
	Vector3 _MaximumVelocity;

	//Some padding.
	Padding<1> _Padding5;

	//The world position.
	Vector3 _WorldPosition;

	//Some padding.
	Padding<1> _Padding6;

	/*
	*	Constructor taking the regular particle system properties.
	*/
	VulkanParticleSystemProperties(const ParticleSystemProperties &otherProperties) NOEXCEPT
		:
		_FadeTime(otherProperties._FadeTime),
		_Lifetime(otherProperties._Lifetime),
		_SpawnFrequency(otherProperties._SpawnFrequency),
		_MinimumScale(otherProperties._MinimumScale),
		_MaximumScale(otherProperties._MaximumScale),
		_MinimumPosition(otherProperties._MinimumPosition),
		_MaximumPosition(otherProperties._MaximumPosition),
		_MinimumVelocity(otherProperties._MinimumVelocity),
		_MaximumVelocity(otherProperties._MaximumVelocity),
		_WorldPosition(otherProperties._WorldPosition)
	{

	}

};

static_assert(offsetof(VulkanParticleSystemProperties, _FadeTime) == 0, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _Lifetime) == 4, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _SpawnFrequency) == 8, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _MinimumScale) == 16, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _MaximumScale) == 24, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _MinimumPosition) == 32, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _MaximumPosition) == 48, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _MinimumVelocity) == 64, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _MaximumVelocity) == 80, "Member of VulkanParticleSystemProperties is not layout correctly.");
static_assert(offsetof(VulkanParticleSystemProperties, _WorldPosition) == 96, "Member of VulkanParticleSystemProperties is not layout correctly.");

static_assert(sizeof(VulkanParticleSystemProperties) == 112, "VulkanParticleSystemProperties has incorrect size.");