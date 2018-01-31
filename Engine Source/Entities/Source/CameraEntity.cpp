//Header file.
#include <CameraEntity.h>

//Static variable definitions.
DynamicArray<CameraEntity *CATALYST_RESTRICT> CameraEntity::instances;

/*
*	Default constructor.
*/
CameraEntity::CameraEntity() CATALYST_NOEXCEPT
{
	//Add this camera entity to the universal container.
	instances.Emplace(this);
}

/*
*	Default destructor.
*/
CameraEntity::~CameraEntity() CATALYST_NOEXCEPT
{
	//Remove this camera entity from the universal container.
	instances.Erase(this);
}