#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/Geometry/Sphere.h>

class DebugRenderingSystem final
{

public:

	/*
	*	Debug render class definition.
	*/
	class DebugRender final
	{

	public:

		//The color.
		Vector4<float32> _Color;

		//Whether or not to perform depth test.
		bool _DepthTest;

		//Whether or not to render as wireframe.
		bool _Wireframe;

	};

	/*
	*	Debug axis aligned bounding box 3D render class definition.
	*/
	class DebugAxisAlignedBoundingBox3DRender final
	{

	public:

		//The debug render.
		DebugRender _DebugRender;

		//The axis aligned bounding box 3D.
		AxisAlignedBoundingBox3 _AxisAlignedBoundingBox3D;

		//The current lifetime.
		float32 _CurrentLifetime;

		//The maximum lifetime.
		float32 _MaximumLifetime;

	};

	/*
	*	Debug sphere render class definition.
	*/
	class DebugSphereRender final
	{

	public:

		//The debug render.
		DebugRender _DebugRender;

		//The sphere.
		Sphere _Sphere;

		//The number of segments.
		uint32 _NumberOfSegments;

		//The current lifetime.
		float32 _CurrentLifetime;

		//The maximum lifetime.
		float32 _MaximumLifetime;

	};

	/*
	*	Initializes the debug rendering system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Debug renders an axis aligned bounding box 3D.
	*/
	void DebugRenderAxisAlignedBoundingBox3D(	const Vector4<float32> &color,
												const bool depth_test,
												const bool wireframe,
												const AxisAlignedBoundingBox3 &axis_aligned_bounding_box_3D,
												const float32 lifetime) NOEXCEPT;

	/*
	*	Debug renders a sphere.
	*/
	void DebugRenderSphere(	const Vector4<float32> &color,
							const bool depth_test,
							const bool wireframe,
							const Sphere &sphere,
							const uint32 number_of_segments,
							const float32 lifetime) NOEXCEPT;

	/*
	*	Returns the debug axis aligned bounding box 3D renders lock.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Spinlock *const RESTRICT GetDebugAxisAlignedBoundingBox3DRendersLock() NOEXCEPT
	{
		return &_DebugAxisAlignedBoundingBox3DRendersLock;
	}

	/*
	*	Returns the debug axis aligned bounding box 3D renders.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<DebugAxisAlignedBoundingBox3DRender> &GetDebugAxisAlignedBoundingBox3DRenders() const NOEXCEPT
	{
		return _DebugAxisAlignedBoundingBox3DRenders;
	}

	/*
	*	Returns the debug sphere renders lock.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Spinlock *const RESTRICT GetDebugSphereRendersLock() NOEXCEPT
	{
		return &_DebugSphereRendersLock;
	}

	/*
	*	Returns the debug sphere renders.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<DebugSphereRender> &GetDebugSphereRenders() const NOEXCEPT
	{
		return _DebugSphereRenders;
	}

private:

	//The debug axis aligned bounding box 3D renders lock.
	Spinlock _DebugAxisAlignedBoundingBox3DRendersLock;

	//Container for all debug axis aligned bounding box 3D renders.
	DynamicArray<DebugAxisAlignedBoundingBox3DRender> _DebugAxisAlignedBoundingBox3DRenders;

	//The debug sphere renders lock.
	Spinlock _DebugSphereRendersLock;

	//Container for all debug sphere renders.
	DynamicArray<DebugSphereRender> _DebugSphereRenders;

	/*
	*	Updates the debug rendering system during the post update phase.
	*/
	void PostUpdate() NOEXCEPT;
	
};
#endif