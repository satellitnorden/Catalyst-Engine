#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/General/EulerAngles.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//World.
#include <World/Core/WorldPosition.h>

class UserInterfaceEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceEntity() NOEXCEPT;

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
	RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT;

	/*
	*	Returns the user interface scene.
	*/
	RESTRICTED NO_DISCARD const UserInterfaceScene *const RESTRICT GetUserInterfaceScene() const NOEXCEPT;

	/*
	*	Sets the user interface scene.
	*/
	void SetUserInterfaceScene(UserInterfaceScene *const RESTRICT value) NOEXCEPT;

	/*
	*	Returns the world position.
	*/
	NO_DISCARD const WorldPosition &GetWorldPosition() const NOEXCEPT;

	/*
	*	Sets the world position.
	*/
	void SetWorldPosition(const WorldPosition &value) NOEXCEPT;

	/*
	*	Returns the rotation.
	*/
	NO_DISCARD const EulerAngles &GetRotation() const NOEXCEPT;

	/*
	*	Sets the rotation.
	*/
	void SetRotation(const EulerAngles &value) NOEXCEPT;

	/*
	*	Returns the scale.
	*/
	NO_DISCARD const Vector2<float32> &GetScale() const NOEXCEPT;

	/*
	*	Sets the scale.
	*/
	void SetScale(const Vector2<float32> &value) NOEXCEPT;

	/*
	*	Returns the roughness.
	*/
	NO_DISCARD float32 GetRoughness() const NOEXCEPT;

	/*
	*	Sets the roughness.
	*/
	void SetRoughness(const float32 value) NOEXCEPT;

	/*
	*	Returns the metallic.
	*/
	NO_DISCARD float32 GetMetallic() const NOEXCEPT;

	/*
	*	Sets the metallic.
	*/
	void SetMetallic(const float32 value) NOEXCEPT;

	/*
	*	Returns the ambient occlusion.
	*/
	NO_DISCARD float32 GetAmbientOcclusion() const NOEXCEPT;

	/*
	*	Sets the ambient occlusion.
	*/
	void SetAmbientOcclusion(const float32 value) NOEXCEPT;

	/*
	*	Returns the emissive multiplier.
	*/
	NO_DISCARD float32 GetEmissiveMultiplier() const NOEXCEPT;

	/*
	*	Sets the emissive multiplier.
	*/
	void SetEmissiveMultiplier(const float32 value) NOEXCEPT;

};