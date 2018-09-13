#if !defined(CATALYST_FINAL)
//Header file.
#include <Systems/DebugRenderingSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Entities.
#include <Entities/CameraEntity.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(DebugRenderingSystem);

/*
*	Updates the debug rendering system synchronously during the opening update phase.
*/
void DebugRenderingSystem::RenderUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Sort the axis-aligned bounding box debug rendering data.
	if (_AxisAlignedBoundingBoxDebugRenderData.Size() > 1)
	{
		SortingAlgorithms::StandardSort<AxisAlignedBoundingBoxDebugRenderData>(_AxisAlignedBoundingBoxDebugRenderData.Begin(), _AxisAlignedBoundingBoxDebugRenderData.End(), [](const AxisAlignedBoundingBoxDebugRenderData *const RESTRICT first, const AxisAlignedBoundingBoxDebugRenderData *const RESTRICT second)
		{
			if (const CameraEntity *const RESTRICT activeCamera{ RenderingSystem::Instance->GetActiveCamera() })
			{
				const Vector3 &cameraWorldPosition{ activeCamera->GetPosition() };

				return Vector3::LengthSquared(cameraWorldPosition - AxisAlignedBoundingBox::CalculateCenter(first->_Box)) < Vector3::LengthSquared(cameraWorldPosition - AxisAlignedBoundingBox::CalculateCenter(second->_Box));
			}

			else
			{
				return true;
			}
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
#endif