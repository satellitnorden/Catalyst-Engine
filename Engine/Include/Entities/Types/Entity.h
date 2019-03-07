#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/EntityCore.h>
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>
#include <Math/Matrix4.h>

class Entity
{

public:

	//The type.
	EntityType _Type{ EntityType::NumberOfEntityTypes };

	//Denotes whether or not this entity is initialized.
	bool _Initialized{ false };

	//The components index for this entity.
	uint64 _ComponentsIndex;

	//Pointer to this entity's parent.
	Entity *RESTRICT _Parent{ nullptr };

	//Pointers to this entity's children.
	DynamicArray<Entity *RESTRICT> _Children;

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the local transform of this entity.
	*/
	Matrix4 *const RESTRICT GetLocalTransform() NOEXCEPT;

	/*
	*	Returns the world transform of this entity.
	*/
	Matrix4 *const RESTRICT GetWorldTransform() NOEXCEPT;

	/*
	*	Transforms this entity.
	*/
	void Transform(const Matrix4 &transformation) NOEXCEPT;

protected:

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	virtual bool ShouldAutomaticallyTerminate() const NOEXCEPT
	{
		return false;
	}

	/*
	*	Callback for when this entity is moved.
	*/
	virtual void OnMove() NOEXCEPT
	{

	}

private:

	//Friend declaration.
	friend class EntityCreationSystem;

	//The default position.
	static constexpr Vector3<float> _DefaultPosition{ 0.0f, 0.0f, 0.0f };

	//The default rotation.
	static constexpr Vector3<float> _DefaultRotation{ 0.0f, 0.0f, 0.0f };

	//The default scale.
	static constexpr Vector3<float> _DefaultScale{ 1.0f, 1.0f, 1.0f };

};