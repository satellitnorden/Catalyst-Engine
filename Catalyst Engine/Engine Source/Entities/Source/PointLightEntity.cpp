//Header file.
#include <PointLightEntity.h>

//Static variable definitions.
DynamicArray<PointLightEntity *CATALYST_RESTRICT> PointLightEntity::pointLightEntities;

/*
*	Default constructor.
*/
PointLightEntity::PointLightEntity() CATALYST_NOEXCEPT
{
	//Add this point light entity to the universal container.
	pointLightEntities.Emplace(this);
}

/*
*	Default destructor.
*/
PointLightEntity::~PointLightEntity() CATALYST_NOEXCEPT
{
	//Remove this point light entity from the universal container.
	for (PointLightEntity * CATALYST_RESTRICT pointLightEntity : pointLightEntities)
	{
		if (pointLightEntity == this)
		{
			pointLightEntity = pointLightEntities.Back();
			pointLightEntities.Pop();

			return;
		}
	}
}