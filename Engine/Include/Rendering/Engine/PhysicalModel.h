#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/RenderingCore.h>

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
	*	Returns the material.
	*/
	const PhysicalMaterial& GetMaterial() const NOEXCEPT { return material; }

	/*
	*	Sets the material
	*/
	void SetMaterial(const PhysicalMaterial &newMaterial) NOEXCEPT { material = newMaterial; }

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
	ConstantBufferHandle GetBuffer() const NOEXCEPT { return buffer; }

	/*
	*	Sets the buffer.
	*/
	void SetBuffer(ConstantBufferHandle newBuffer) NOEXCEPT { buffer = newBuffer; }

	/*
	*	Returns the index offset.
	*/
	uint64 GetIndexOffset() const NOEXCEPT { return indexOffset; }

	/*
	*	Sets the index offset.
	*/
	void SetIndexOffset(const uint64 newIndexOffset) NOEXCEPT { indexOffset = newIndexOffset; }

	/*
	*	Returns the index count.
	*/
	uint32 GetIndexCount() const NOEXCEPT { return indexCount; }

	/*
	*	Sets the index count.
	*/
	void SetIndexCount(const uint32 newIndexCount) NOEXCEPT { indexCount = newIndexCount; }

private:

	//The material.
	PhysicalMaterial material;

	//The axis aligned bounding box of this physical model.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

	//The buffer.
	ConstantBufferHandle buffer;

	//The index offset in the buffer.
	uint64 indexOffset;

	//The number of indices that this physical model has.
	uint32 indexCount;

};