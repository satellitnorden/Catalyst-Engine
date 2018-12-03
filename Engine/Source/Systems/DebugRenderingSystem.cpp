#if !defined(CATALYST_FINAL)
//Header file.
#include <Systems/DebugRenderingSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugRenderingSystem);

/*
*	Updates the debug rendering system synchronously during the rendering update phase.
*/
void DebugRenderingSystem::RenderingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Sort the axis-aligned bounding box debug rendering data.
	if (_AxisAlignedBoundingBoxDebugRenderData.Size() > 1)
	{
		SortingAlgorithms::InsertionSort<AxisAlignedBoundingBoxDebugRenderData>(_AxisAlignedBoundingBoxDebugRenderData.Begin(), _AxisAlignedBoundingBoxDebugRenderData.End(), nullptr, [](const void *const RESTRICT userData, const AxisAlignedBoundingBoxDebugRenderData *const RESTRICT first, const AxisAlignedBoundingBoxDebugRenderData *const RESTRICT second)
		{
			const Vector3<float> &viewerPosition{ Viewer::Instance->GetPosition() };

			return Vector3<float>::LengthSquared(viewerPosition - AxisAlignedBoundingBox::CalculateCenter(first->_Box)) < Vector3<float>::LengthSquared(viewerPosition - AxisAlignedBoundingBox::CalculateCenter(second->_Box));
		});
	}
}

/*
*	Updates the debug rendering system synchronously during the closing update phase.
*/
void DebugRenderingSystem::ClosingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Clear all data.
	_AxisAlignedBoundingBoxDebugRenderData.ClearFast();
	_ScreenBoxDebugRenderData.ClearFast();
}

/*
*	Debug renders an axis-aligned bounding box.
*/
void DebugRenderingSystem::DebugRenderAxisAlignedBoundingBox(const AxisAlignedBoundingBoxDebugRenderData &data) NOEXCEPT
{
	//Lock the axis-aligned bounding box debug render data.
	ScopedLock<Spinlock> scopedLock{ _AxisAlignedBoundingBoxDebugRenderDataLock };

	//Add the data to the axis-aligned bounding box debug render data.
	_AxisAlignedBoundingBoxDebugRenderData.EmplaceSlow(data);
}

/*
*	Debug renders a screen box.
*/
void DebugRenderingSystem::DebugRenderScreenBox(const ScreenBoxDebugRenderData &data) NOEXCEPT
{
	//Lock the screen box debug render data.
	ScopedLock<Spinlock> scopedLock{ _ScreenBoxDebugRenderDataLock };

	//Add the data to the screen box debug render data.
	_ScreenBoxDebugRenderData.EmplaceSlow(data);
}
#endif