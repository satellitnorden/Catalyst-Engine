#pragma once

//Engine core.
#include <EngineCore.h>

//Entities.
#include <Entity.h>

class TerrainEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(TerrainEntity);

	/*
	*	Default constructor.
	*/
	TerrainEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~TerrainEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() CATALYST_NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() CATALYST_NOEXCEPT final override;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &moveVector) CATALYST_NOEXCEPT final override;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT final override;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT final override;

};