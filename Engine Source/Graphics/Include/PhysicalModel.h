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
	PhysicalModel() CATALYST_NOEXCEPT;

	/*
	*	Copy constructor.
	*/
	PhysicalModel(const PhysicalModel &otherPhysicalModel) CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~PhysicalModel() CATALYST_NOEXCEPT;

	/*
	*	Copy assignment overload.
	*/
	PhysicalModel& operator=(const PhysicalModel &otherPhysicalModel) CATALYST_NOEXCEPT;

	/*
	*	Returns the axis aligned bounding box of this physical model, const.
	*/
	const AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() const CATALYST_NOEXCEPT { return axisAlignedBoundingBox; }

	/*
	*	Returns the axis aligned bounding box of this physical model, non-const.
	*/
	AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() CATALYST_NOEXCEPT { return axisAlignedBoundingBox; }

	/*
	*	Returns the vertex buffer.
	*/
	CATALYST_RESTRICTED const VulkanVertexBuffer* GetVertexBuffer() const CATALYST_NOEXCEPT { return vertexBuffer; }

	/*
	*	Sets the vertex buffer.
	*/
	void SetVertexBuffer(VulkanVertexBuffer *CATALYST_RESTRICT newVertexBuffer) CATALYST_NOEXCEPT { vertexBuffer = newVertexBuffer; }

	/*
	*	Returns the index buffer.
	*/
	CATALYST_RESTRICTED const VulkanIndexBuffer* GetIndexBuffer() const CATALYST_NOEXCEPT { return indexBuffer; }

	/*
	*	Sets  the index buffer.
	*/
	void SetIndexBuffer(VulkanIndexBuffer *CATALYST_RESTRICT newIndexBuffer) CATALYST_NOEXCEPT { indexBuffer = newIndexBuffer; }

	/*
	*	Returns the material, const version.
	*/
	const PhysicalMaterial& GetMaterial() const CATALYST_NOEXCEPT { return material; }

	/*
	*	Returns the material, non-const version.
	*/
	PhysicalMaterial& GetMaterial() CATALYST_NOEXCEPT { return material; }

	/*
	*	Returns the index count.
	*/
	uint32 GetIndexCount() const CATALYST_NOEXCEPT { return indexCount; }

	/*
	*	Sets the index count.
	*/
	void SetIndexCount(const uint32 newIndexCount) CATALYST_NOEXCEPT { indexCount = newIndexCount; }

private:

	//The axis aligned bounding box of this physical model.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

	//The vertex buffer.
	VulkanVertexBuffer *CATALYST_RESTRICT vertexBuffer;

	//The index buffer.
	VulkanIndexBuffer *CATALYST_RESTRICT indexBuffer;

	//The material.
	PhysicalMaterial material;

	//The number of indices that this physical model has.
	uint32 indexCount;

};