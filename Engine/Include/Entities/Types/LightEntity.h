#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class LightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	LightEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	bool ShouldAutomaticallyTerminate() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return false;
	}

	/*
	*	Returns the light type of this light.
	*/
	LightType GetLightType() NOEXCEPT;

	/*
	*	Returns the direction of this light.
	*/
	Vector3<float32> GetDirection() NOEXCEPT;

	/*
	*	Sets the direction of this light.
	*/
	void SetDirection(const Vector3<float32>& direction) NOEXCEPT;

	/*
	*	Returns the position of this light.
	*/
	Vector3<float32> GetPosition() NOEXCEPT;

	/*
	*	Sets the position of this light.
	*/
	void SetPosition(const Vector3<float32> &position) NOEXCEPT;

	/*
	*	Returns the luminance of this light.
	*/
	Vector3<float32> GetLuminance() NOEXCEPT;

	/*
	*	Sets the luminance of this light.
	*/
	void SetLuminance(const Vector3<float32> &luminance) NOEXCEPT;

};