#if !defined(CATALYST_FINAL)
//Header file.
#include <Core/Utilities/DebugUtilities.h>

//Systems.
#include <Systems/DebugRenderingSystem.h>
#include <Systems/TerrainSystem.h>

namespace DebugUtilities
{

	/*
	*	Debug renders all axis aligned bounding boxes for terrain.
	*/
	void DebugRenderTerrainAxisAlignedBoundingBoxes() NOEXCEPT
	{
		constexpr float ALPHA{ 0.25f };
		constexpr StaticArray<Vector4, 6> colors
		{
			Vector4(1.0f, 0.0f, 0.0f, ALPHA),
			Vector4(0.0f, 1.0f, 0.0f, ALPHA),
			Vector4(0.0f, 0.0f, 1.0f, ALPHA),

			Vector4(1.0f, 1.0f, 0.0f, ALPHA),
			Vector4(1.0f, 0.0f, 1.0f, ALPHA),
			Vector4(0.0f, 1.0f, 1.0f, ALPHA),
		};

		uint8 counter{ 0 };

		const StaticArray<TerrainPatchInformation, 9> *const RESTRICT highDetailInformations{ TerrainSystem::Instance->GetHighDetailTerrainPatchInformations() };

		for (const TerrainPatchInformation &information : *highDetailInformations)
		{
			if (information._Valid)
			{
				DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData data;

				data._Box = information._AxisAlignedBoundingBox;
				data._Color = colors[(counter++) % colors.Size()];

				DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(data);
			}
		}

		counter = 0;

		const StaticArray<TerrainPatchInformation, 56> *const RESTRICT lowDetailInformations{ TerrainSystem::Instance->GetLowDetailTerrainPatchInformations() };

		for (const TerrainPatchInformation &information : *lowDetailInformations)
		{
			if (information._Valid)
			{
				DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData data;

				data._Box = information._AxisAlignedBoundingBox;
				data._Color = colors[(counter++) % colors.Size()];

				DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(data);
			}
		}
	}

}
#endif