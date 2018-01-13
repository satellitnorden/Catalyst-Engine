//Header file.
#include <CameraEntity.h>

//Static variable definitions.
DynamicArray<CameraEntity *CATALYST_RESTRICT> CameraEntity::cameraEntities;

/*
*	Default constructor.
*/
CameraEntity::CameraEntity() CATALYST_NOEXCEPT
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