#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Entity.
#include <Entities/Entity.h>

//Forward declarations.
class WaterMaterial;
class WaterUniformData;

class WaterEntity : public Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(WaterEntity);

	/*
	*	Default constructor.
	*/
	WaterEntity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~WaterEntity() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() NOEXCEPT final override;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() NOEXCEPT final override;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() NOEXCEPT final override;

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
	*	Initializes this water entity.
	*/
	void Initialize(const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) NOEXCEPT;

};