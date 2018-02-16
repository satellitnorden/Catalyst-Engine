#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <AxisAlignedBoundingBox.h>
#include <PhysicalMaterial.h>

//Forward declarations.
class VulkanBuffer;

class PhysicalModel final
{

public:

	/*
	*	Default constructor.
	*/
	PhysicalModel() NOEXCEPT;

	/*
	*	Copy constructor.
	*/
	PhysicalModel(const PhysicalModel &otherPhysicalModel) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicalModel() NOEXCEPT;

	/*
	*	Copy assignment overload.
	*/
	PhysicalModel& operator=(const PhysicalModel &otherPhysicalModel) NOEXCEPT;

	/*
	*	Returns the axis aligned bounding box of this physical model, const.
	*/
	const AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() const NOEXCEPT { return axisAlignedBoundingBox; }

	/*
	*	Returns the axis aligned bounding box of this physical model, non-const.
	*/
	AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() NOEXCEPT { return axisAlignedBoundingBox; }

	/*
	*	Returns the buffer.
	*/
	RESTRICTED const VulkanBuffer* GetBuffer() const NOEXCEPT { return buffer; }

	/*
	*	Sets the buffer.
	*/
	void SetBuffer(VulkanBuffer *RESTRICT newBuffer) NOEXCEPT { buffer = newBuffer; }

	/*
	*	Returns the index offset.
	*/
	uint64 GetIndexOffset() const NOEXCEPT { return indexOffset; }

	/*
	*	Sets the index offset.
	*/
	void SetIndexOffset(const uint64 newIndexOffset) NOEXCEPT { indexOffset = newIndexOffset; }

	/*
	*	Returns the material, const version.
	*/
	const PhysicalMaterial& GetMaterial() const NOEXCEPT { return material; }

	/*
	*	Returns the material, non-const version.
	*/
	PhysicalMaterial& GetMaterial() NOEXCEPT { return material; }

	/*
	*	Returns the index count.
	*/
	uint32 GetIndexCount() const NOEXCEPT { return indexCount; }

	/*
	*	Sets the index count.
	*/
	void SetIndexCount(const uint32 newIndexCount) NOEXCEPT { indexCount = newIndexCount; }

private:

	//The axis aligned bounding box of this physical model.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

	//The buffer.
	VulkanBuffer *RESTRICT buffer;

	//The index offset in the buffer.
	uint64 indexOffset;

	//The material.
	PhysicalMaterial material;

	//The number of indices that this physical model has.
	uint32 indexCount;

};