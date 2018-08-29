#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Entity.
#include <Entities/Entity.h>

//Rendering
#include <Rendering/Engine/VegetationProperties.h>
#include <Rendering/Engine/VegetationTransformation.h>

//Forward declarations.
class VegetationMaterial;

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
	void Initialize(const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) NOEXCEPT;

};