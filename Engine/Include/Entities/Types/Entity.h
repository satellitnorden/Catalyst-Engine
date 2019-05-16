#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/EntityCore.h>
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/General/Matrix.h>

class Entity
{

public:

	//The type.
	EntityType _Type{ EntityType::NumberOfEntityTypes };

	//Denotes whether or not this entity is initialized.
	bool _Initialized{ false };

	//The components index for this entity.
	uint64 _ComponentsIndex;

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
	bool ShouldAutomaticallyTerminate() const NOEXCEPT;

	/*
	*	Returns the world transform of this entity.
	*/
	Matrix4 *const RESTRICT GetWorldTransform() NOEXCEPT;

private:

	//The default position.
	static constexpr Vector3<float> _DefaultPosition{ 0.0f, 0.0f, 0.0f };

	//The default rotation.
	static constexpr Vector3<float> _DefaultRotation{ 0.0f, 0.0f, 0.0f };

	//The default scale.
	static constexpr Vector3<float> _DefaultScale{ 1.0f, 1.0f, 1.0f };

};