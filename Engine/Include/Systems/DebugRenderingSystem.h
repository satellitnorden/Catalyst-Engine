#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>
#include <Math/General/Vector.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

class DebugRenderingSystem final
{

public:

	/*
	*	Axis-aligned bounding box render data definition.
	*/
	class AxisAlignedBoundingBoxDebugRenderData final
	{

	public:

		//The axis-aligned bounding box to render.
		AxisAlignedBoundingBox _Box;

		//The color.
		Vector4<float> _Color;

		/*
		*	Default constructor.
		*/
		AxisAlignedBoundingBoxDebugRenderData() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		AxisAlignedBoundingBoxDebugRenderData(const AxisAlignedBoundingBox &initialBox, const Vector4<float> &initialColor) NOEXCEPT
			:
			_Box(initialBox),
			_Color(initialColor)
		{

		}

	};

	/*
	*	Screen box render data definition.
	*/
	class ScreenBoxDebugRenderData final
	{

	public:

		//The minimum of the box.
		Vector2<float> _Minimum;

		//The maximum of the box.
		Vector2<float> _Maximum;

		//The color.
		Vector4<float> _Color;

		/*
		*	Default constructor.
		*/
		ScreenBoxDebugRenderData() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		ScreenBoxDebugRenderData(const Vector2<float> &initialMinimum, const Vector2<float> &initialMaximum, const Vector4<float> &initialColor) NOEXCEPT
			:
			_Minimum(initialMinimum),
			_Maximum(initialMaximum),
			_Color(initialColor)
		{

		}

	};

	//Singleton declaration.
	DECLARE_SINGLETON(DebugRenderingSystem);

	/*
	*	Default constructor.
	*/
	DebugRenderingSystem() NOEXCEPT
	{

	}

	/*
	*	Updates the debug rendering system system synchronously.
	*/
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Post-updates the update system synchronously.
	*/
	void PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the axis-aligned bounding box debug render data.
	*/
	RESTRICTED const DynamicArray<AxisAlignedBoundingBoxDebugRenderData> *const RESTRICT GetAxisAlignedBoundingBoxDebugRenderData() const NOEXCEPT
	{
		return &_AxisAlignedBoundingBoxDebugRenderData;
	}

	/*
	*	Returns the screen box debug render data.
	*/
	RESTRICTED const DynamicArray<ScreenBoxDebugRenderData> *const RESTRICT GetScreenBoxDebugRenderData() const NOEXCEPT
	{
		return &_ScreenBoxDebugRenderData;
	}

	/*
	*	Debug renders an axis-aligned bounding box.
	*/
	void DebugRenderAxisAlignedBoundingBox(const AxisAlignedBoundingBoxDebugRenderData &data) NOEXCEPT;

	/*
	*	Debug renders a screen box.
	*/
	void DebugRenderScreenBox(const ScreenBoxDebugRenderData &data) NOEXCEPT;

private:

	//The axis-aligned bounding box debug render data lock.
	Spinlock _AxisAlignedBoundingBoxDebugRenderDataLock;

	//Container for all axis-aligned bounding box debug render data.
	DynamicArray<AxisAlignedBoundingBoxDebugRenderData> _AxisAlignedBoundingBoxDebugRenderData;

	//The screen box debug render data lock.
	Spinlock _ScreenBoxDebugRenderDataLock;

	//Container for all screen box debug render data.
	DynamicArray<ScreenBoxDebugRenderData> _ScreenBoxDebugRenderData;

};

#endif
