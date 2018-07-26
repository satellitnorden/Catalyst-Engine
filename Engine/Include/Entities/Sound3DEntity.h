#pragma once

//Core.
#include <Core/EngineCore.h>

//Entities.
#include <Entities/Entity.h>

//Sound.
#include <Sound/SoundCore.h>

class Sound3DEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(Sound3DEntity);

	/*
	*	Default constructor.
	*/
	Sound3DEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~Sound3DEntity() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() const NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() const NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() const NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) NOEXCEPT final override;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &scaleVector) NOEXCEPT final override;

	/*
	*	Initializes this sound 3D entity.
	*/
	void Initialize(const FMOD::Studio::EventDescription *const RESTRICT eventDescription) NOEXCEPT;

};