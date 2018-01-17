#pragma once

//Engine core.
#include <EngineCore.h>

//Entity.
#include <Entity.h>

//Physics.
#include <PhysicalConstants.h>

class CameraEntity : public Entity
{

public:

	//Universal container of all camera entities.
	static DynamicArray<CameraEntity *CATALYST_RESTRICT> cameraEntities;

	/*
	*	Default constructor.
	*/
	CameraEntity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~CameraEntity() CATALYST_NOEXCEPT;

	/*
	*	Returns the near plane of the frustum.
	*/
	const float& GetNearPlane() const CATALYST_NOEXCEPT { return nearPlane; }

	/*
	*	Returns the far plane of the frustum.
	*/
	const float& GetFarPlane() const CATALYST_NOEXCEPT { return farPlane; }

	/*
	*	Returns the field of view for this camera entity.
	*/
	const float& GetFieldOfView() const CATALYST_NOEXCEPT { return fieldOfView; }

private:

	//Defines the near plane of the frustum.
	float nearPlane{ 0.1f };

	//Defines the far plane of the frustum.
	float farPlane{ PHYSICAL_CONSTANT_VIEW_DISTANCE };

	//Defines the field of view of this camera entity.
	float fieldOfView{ 60.0f };

};