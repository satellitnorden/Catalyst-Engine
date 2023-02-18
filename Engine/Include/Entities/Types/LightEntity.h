#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Core/WorldPosition.h>

class LightEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	LightEntity() NOEXCEPT;

	/*
	*	Returns the preprocessing parameters.
	*/
	FORCE_INLINE void GetPreprocessingParameters(EntityPreprocessingParameters* const RESTRICT parameters) NOEXCEPT
	{

	}

	/*
	*	Preprocesses this entity.
	*/
	FORCE_INLINE void Preprocess(EntityInitializationData* const RESTRICT data) NOEXCEPT
	{

	}

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
	*	Returns the initialization data required to duplicate this entity.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return nullptr;
	}

	/*
	*	Returns the rotation of this light.
	*/
	EulerAngles GetRotation() const NOEXCEPT;

	/*
	*	Sets the rotation of this light.
	*/
	void SetRotation(const EulerAngles &rotation) NOEXCEPT;

	/*
	*	Returns the world position of this light.
	*/
	NO_DISCARD const WorldPosition &GetWorldPosition() const NOEXCEPT;

	/*
	*	Sets the world position of this light.
	*/
	void SetWorldPosition(const WorldPosition &world_position) NOEXCEPT;

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