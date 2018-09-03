//Header file.
#include <Rendering/Engine/PhysicalModel.h>

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
	_AxisAlignedBoundingBox(otherPhysicalModel._AxisAlignedBoundingBox),
	_Buffer(otherPhysicalModel._Buffer),
	_IndexOffset(otherPhysicalModel._IndexOffset),
	_Material(otherPhysicalModel._Material),
	_IndexCount(otherPhysicalModel._IndexCount)
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
	_AxisAlignedBoundingBox = otherPhysicalModel._AxisAlignedBoundingBox;
	_Buffer = otherPhysicalModel._Buffer;
	_IndexOffset = otherPhysicalModel._IndexOffset;
	_Material = otherPhysicalModel._Material;
	_IndexCount = otherPhysicalModel._IndexCount;

	return *this;
}