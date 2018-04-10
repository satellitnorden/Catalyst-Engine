#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Entity.
#include <Entities/Entity.h>

class VegetationEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(VegetationEntity);

	/*
	*	Default constructor.
	*/
	VegetationEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~VegetationEntity() NOEXCEPT;

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
	*	Initializes this vegetation entity.
	*/
	void Initialize(const DynamicArray<Vector3> &positions) NOEXCEPT;

};