#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Vector.h>

class ParticleSystemInitializationData final : public EntityInitializationData
{

public:

	//The global material index.
	uint32 _GlobalMaterialIndex;

	//The position extent.
	Vector3<float> _PositionExtent;

	//The velocity extent.
	Vector3<float> _VelocityExtent;

	//The size extent.
	Vector2<float> _SizeExtent;

	//The fade time.
	float _FadeTime;

	//The lifetime.
	float _Lifetime;

};