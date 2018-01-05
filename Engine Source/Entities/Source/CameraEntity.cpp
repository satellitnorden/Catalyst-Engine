//Header file.
#include <CameraEntity.h>

//Physics.
#include <PhysicalConstants.h>

//Static variable definitions.
DynamicArray<CameraEntity *CATALYST_RESTRICT> CameraEntity::cameraEntities;

/*
*	Default constructor.
*/
CameraEntity::CameraEntity() CATALYST_NOEXCEPT
	:
	nearPlane(0.1f),
	farPlane(PHYSICAL_CONSTANT_VIEW_DISTANCE),
	fieldOfView(60.0f)
{
	//Add this camera entity to the universal container.
	cameraEntities.Emplace(this);
}

/*
*	Default destructor.
*/
CameraEntity::~CameraEntity() CATALYST_NOEXCEPT
{
	//Remove this camera entity from the universal container.
	cameraEntities.Erase(this);
}