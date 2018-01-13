#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <PhysicalMaterial.h>

//Forward declarations.
class VulkanIndexBuffer;
class VulkanVertexBuffer;

class PhysicalModel
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
	*	Returns the extent of this physical model.
	*/
	CATALYST_FORCE_INLINE float GetExtent() const CATALYST_NOEXCEPT { return extent; }

	/*
	*	Sets extent of this physical model.
	*/
	CATALYST_FORCE_INLINE void SetExtent(const float newExtent) CATALYST_NOEXCEPT { extent = newExtent; }

	/*
	*	Returns the vertex buffer.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanVertexBuffer* GetVertexBuffer() const CATALYST_NOEXCEPT { return vertexBuffer; }

	/*
	*	Sets the vertex buffer.
	*/
	CATALYST_FORCE_INLINE void SetVertexBuffer(VulkanVertexBuffer *CATALYST_RESTRICT newVertexBuffer) CATALYST_NOEXCEPT { vertexBuffer = newVertexBuffer; }

	/*
	*	Returns the index buffer.
	*/
	CATALYST_FORCE_INLINE CATALYST_RESTRICTED const VulkanIndexBuffer* GetIndexBuffer() const CATALYST_NOEXCEPT { return indexBuffer; }

	/*
	*	Sets  the index buffer.
	*/
	CATALYST_FORCE_INLINE void SetIndexBuffer(VulkanIndexBuffer *CATALYST_RESTRICT newIndexBuffer) CATALYST_NOEXCEPT { indexBuffer = newIndexBuffer; }

	/*
	*	Returns the material, const version.
	*/
	CATALYST_FORCE_INLINE const PhysicalMaterial& GetMaterial() const CATALYST_NOEXCEPT { return material; }

	/*
	*	Returns the material, non-const version.
	*/
	CATALYST_FORCE_INLINE PhysicalMaterial& GetMaterial() CATALYST_NOEXCEPT { return material; }

	/*
	*	Returns the index count.
	*/
	CATALYST_FORCE_INLINE uint32 GetIndexCount() const CATALYST_NOEXCEPT { return indexCount; }

	/*
	*	Sets the index count.
	*/
	CATALYST_FORCE_INLINE void SetIndexCount(const uint32 newIndexCount) CATALYST_NOEXCEPT { indexCount = newIndexCount; }

private:

	//The extent of this physical model.
	std::atomic<float> extent;

	//The vertex buffer.
	VulkanVertexBuffer *CATALYST_RESTRICT vertexBuffer;

	//The index buffer.
	VulkanIndexBuffer *CATALYST_RESTRICT indexBuffer;

	//The material.
	PhysicalMaterial material;

	//The number of indices that this physical model has.
	uint32 indexCount;

};