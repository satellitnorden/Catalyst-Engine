#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>
#include <Entities/Types/ParticleSystemEntity.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldPosition.h>

class ParticleSystemInitializationData final : public EntityInitializationData
{

public:

	//The particle system properties.
	ParticleSystemEntity::Property _ParticleSystemProperties;

	//The initial world position.
	WorldPosition _InitialWorldPosition;

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

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

	//The initial spawn frequency, defined in particles per seconds.
	float32 _InitialSpawnFrequency;

	//The lifetime.
	float32 _Lifetime;

	//The fade time.
	float32 _FadeTime;

};