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
	*	Returns the direction of this light.
	*/
	Vector3<float32> GetDirection() const NOEXCEPT;

	/*
	*	Sets the direction of this light.
	*/
	void SetDirection(const Vector3<float32>& direction) NOEXCEPT;

	/*
	*	Returns the position of this light.
	*/
	Vector3<float32> GetPosition() const NOEXCEPT;

	/*
	*	Sets the position of this light.
	*/
	void SetPosition(const Vector3<float32> &position) NOEXCEPT;

	/*
	*	Returns the color of this light.
	*/
	Vector3<float32> GetColor() const NOEXCEPT;

	/*
	*	Sets the color of this light.
	*/
	void SetColor(const Vector3<float32> &color) NOEXCEPT;

	/*
	*	Returns the light type of this light.
	*/
	LightType GetLightType() const NOEXCEPT;

	/*
	*	Sets the light type of this light.
	*/
	void SetLightType(const LightType type) NOEXCEPT;

	/*
	*	Returns the light properties of this light.
	*/
	uint32 GetLightProperties() const NOEXCEPT;

	/*
	*	Sets the light properties of this light.
	*/
	void SetLightProperties(const uint32 properties) NOEXCEPT;

	/*
	*	Returns the intensity of this light.
	*/
	float32 GetIntensity() const NOEXCEPT;

	/*
	*	Sets the intensity of this light.
	*/
	void SetIntensity(const float32 intensity) NOEXCEPT;

	/*
	*	Returns the radius of this light.
	*/
	float32 GetRadius() const NOEXCEPT;

	/*
	*	Sets the radius of this light.
	*/
	void SetRadius(const float32 radius) NOEXCEPT;

	/*
	*	Returns the size of this light.
	*/
	float32 GetSize() const NOEXCEPT;

	/*
	*	Sets the size of this light.
	*/
	void SetSize(const float32 size) NOEXCEPT;

	/*
	*	Returns if this light is surface shadow casting.
	*/
	NO_DISCARD bool IsSurfaceShadowCasting() const NOEXCEPT;

	/*
	*	Sets is this light is surface shadow casting.
	*/
	void SetIsSurfaceShadowCasting(const bool is_surface_shadow_casting) NOEXCEPT;

	/*
	*	Returns if this light is volumetric.
	*/
	NO_DISCARD bool IsVolumetric() const NOEXCEPT;

	/*
	*	Sets is this light is volumetric.
	*/
	void SetIsVolumetric(const bool is_volumetric) NOEXCEPT;

	/*
	*	Returns if this light is volumetric shadow casting.
	*/
	NO_DISCARD bool IsVolumetricShadowCasting() const NOEXCEPT;

	/*
	*	Sets is this light is volumetric shadow casting.
	*/
	void SetIsVolumetricShadowCasting(const bool is_volumetric_shadow_casting) NOEXCEPT;

};