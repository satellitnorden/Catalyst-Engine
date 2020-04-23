#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Math.
#include <Math/Geometry/Sphere.h>

class DebugRenderingSystem final
{

public:

	/*
	*	Debug sphere render class definition.
	*/
	class DebugSphereRender final
	{

	public:

		//The sphere.
		Sphere _Sphere;

		//The color.
		Vector4<float32> _Color;

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
	*	Debug renders a sphere.
	*/
	void DebugRenderSphere(	const Sphere &sphere,
							const Vector4<float32> &color,
							const uint32 number_of_segments,
							const float32 lifetime) NOEXCEPT;

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