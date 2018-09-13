#if !defined(CATALYST_FINAL)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Pointers/UniquePointer.h>

//Math.
#include <Math/Vector4.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

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
		Vector4 _Color;

		/*
		*	Default constructor.
		*/
		AxisAlignedBoundingBoxDebugRenderData() NOEXCEPT
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		AxisAlignedBoundingBoxDebugRenderData(const AxisAlignedBoundingBox &initialBox, const Vector4 &initialColor) NOEXCEPT
			:
			_Box(initialBox),
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
	*	Updates the debug rendering system synchronously during the render update phase.
	*/
	void RenderUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the debug rendering system synchronously during the closing update phase.
	*/
	void ClosingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the axis-aligned bounding box debug render data.
	*/
	RESTRICTED const DynamicArray<AxisAlignedBoundingBoxDebugRenderData> *const RESTRICT GetAxisAlignedBoundingBoxDebugRenderData() const NOEXCEPT
	{
		return &_AxisAlignedBoundingBoxDebugRenderData;
	}

	/*
	*	Debug renders an axis-aligned bounding box.
	*/
	void DebugRenderAxisAlignedBoundingBox(const AxisAlignedBoundingBoxDebugRenderData &data) NOEXCEPT;

private:

	//The axis-aligned bounding box debug render data lock.
	Spinlock _AxisAlignedBoundingBoxDebugRenderDataLock;

	//Container for all axis-aligned bounding box debug render data.
	DynamicArray<AxisAlignedBoundingBoxDebugRenderData> _AxisAlignedBoundingBoxDebugRenderData;

};

#endif
