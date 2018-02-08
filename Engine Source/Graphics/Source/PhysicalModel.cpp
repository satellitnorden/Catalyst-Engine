//Header file.
#include <PhysicalModel.h>

/*
*	Default constructor.
*/
PhysicalModel::PhysicalModel() CATALYST_NOEXCEPT
{

}

/*
*	Copy constructor.
*/
PhysicalModel::PhysicalModel(const PhysicalModel &otherPhysicalModel) CATALYST_NOEXCEPT
	:
	extent(otherPhysicalModel.extent.load()),
	vertexBuffer(otherPhysicalModel.vertexBuffer),
	indexBuffer(otherPhysicalModel.indexBuffer),
	material(otherPhysicalModel.material),
	indexCount(otherPhysicalModel.indexCount)
{

}

/*
*	Default destructor.
*/
PhysicalModel::~PhysicalModel() CATALYST_NOEXCEPT
{

}

/*
*	Copy assignment overload.
*/
PhysicalModel& PhysicalModel::operator=(const PhysicalModel &otherPhysicalModel) CATALYST_NOEXCEPT
{
	extent = otherPhysicalModel.extent.load();
	vertexBuffer = otherPhysicalModel.vertexBuffer;
	indexBuffer = otherPhysicalModel.indexBuffer;
	material = otherPhysicalModel.material;
	indexCount = otherPhysicalModel.indexCount;

	return *this;
}