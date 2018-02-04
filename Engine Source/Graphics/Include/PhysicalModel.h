#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <AxisAlignedBoundingBox.h>
#include <PhysicalMaterial.h>

//Forward declarations.
class VulkanIndexBuffer;
class VulkanVertexBuffer;

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
	*	Returns the vertex buffer.
	*/
	RESTRICTED const VulkanVertexBuffer* GetVertexBuffer() const NOEXCEPT { return vertexBuffer; }

	/*
	*	Sets the vertex buffer.
	*/
	void SetVertexBuffer(VulkanVertexBuffer *RESTRICT newVertexBuffer) NOEXCEPT { vertexBuffer = newVertexBuffer; }

	/*
	*	Returns the index buffer.
	*/
	RESTRICTED const VulkanIndexBuffer* GetIndexBuffer() const NOEXCEPT { return indexBuffer; }

	/*
	*	Sets  the index buffer.
	*/
	void SetIndexBuffer(VulkanIndexBuffer *RESTRICT newIndexBuffer) NOEXCEPT { indexBuffer = newIndexBuffer; }

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

	//The vertex buffer.
	VulkanVertexBuffer *RESTRICT vertexBuffer;

	//The index buffer.
	VulkanIndexBuffer *RESTRICT indexBuffer;

	//The material.
	PhysicalMaterial material;

	//The number of indices that this physical model has.
	uint32 indexCount;

};