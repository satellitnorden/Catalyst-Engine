#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/EntityCore.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>
#include <Math/Vector3.h>

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
	*	Default destructor.
	*/
	~Entity() NOEXCEPT
	{

	}

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3<float> &amount) NOEXCEPT;

protected:

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	virtual bool ShouldAutomaticallyTerminate() const NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED virtual NO_DISCARD Vector3<float> *const RESTRICT GetPositionInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Returns the rotation of this entity.
	*/
	RESTRICTED virtual NO_DISCARD Vector3<float> *const RESTRICT GetRotationInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Returns the scale of this entity.
	*/
	RESTRICTED virtual NO_DISCARD Vector3<float> *const RESTRICT GetScaleInternal() NOEXCEPT
	{
		return nullptr;
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