#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Physics.
#include <Physics/PhysicsCore.h>

class ModelAxisAlignedBoundingBoxCollisionData final
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

};

class ModelCollisionData final
{

public:

	//The type.
	ModelCollisionType _Type{ ModelCollisionType::NONE };

	union
	{
		//The axis aligned bounding box data.
		ModelAxisAlignedBoundingBoxCollisionData _AxisAlignedBoundingBoxData;
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ModelCollisionData() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE ModelCollisionData(const ModelCollisionData &other) NOEXCEPT
	{
		Memory::Copy(this, &other, sizeof(ModelCollisionData));
	}

	/*
	*	Assignment operator overload.
	*/
	FORCE_INLINE void operator=(const ModelCollisionData &other) NOEXCEPT
	{
		new (this) ModelCollisionData(other);
	}

};