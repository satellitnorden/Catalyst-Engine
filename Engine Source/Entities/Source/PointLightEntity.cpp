//Header file.
#include <PointLightEntity.h>

//Static variable definitions.
DynamicArray<PointLightEntity *CATALYST_RESTRICT> PointLightEntity::instances;

/*
*	Default constructor.
*/
PointLightEntity::PointLightEntity() CATALYST_NOEXCEPT
{
	//Add this point light entity to the universal container.
	instances.Emplace(this);
}

/*
*	Default destructor.
*/
PointLightEntity::~PointLightEntity() CATALYST_NOEXCEPT
{
	//Remove this point light entity from the universal container.
	instances.Erase(this);
}