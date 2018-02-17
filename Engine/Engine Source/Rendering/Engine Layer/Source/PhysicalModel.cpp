//Header file.
#include <PhysicalModel.h>

/*
*	Default constructor.
*/
PhysicalModel::PhysicalModel() NOEXCEPT
{

}

/*
*	Copy constructor.
*/
PhysicalModel::PhysicalModel(const PhysicalModel &otherPhysicalModel) NOEXCEPT
	:
	axisAlignedBoundingBox(otherPhysicalModel.axisAlignedBoundingBox),
	buffer(otherPhysicalModel.buffer),
	indexOffset(otherPhysicalModel.indexOffset),
	material(otherPhysicalModel.material),
	indexCount(otherPhysicalModel.indexCount)
{

}

/*
*	Default destructor.
*/
PhysicalModel::~PhysicalModel() NOEXCEPT
{

}

/*
*	Copy assignment overload.
*/
PhysicalModel& PhysicalModel::operator=(const PhysicalModel &otherPhysicalModel) NOEXCEPT
{
	axisAlignedBoundingBox = otherPhysicalModel.axisAlignedBoundingBox;
	buffer = otherPhysicalModel.buffer;
	indexOffset = otherPhysicalModel.indexOffset;
	material = otherPhysicalModel.material;
	indexCount = otherPhysicalModel.indexCount;

	return *this;
}