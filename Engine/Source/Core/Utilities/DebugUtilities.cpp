#if !defined(CATALYST_FINAL)
//Header file.
#include <Core/Utilities/DebugUtilities.h>

//Systems.
#include <Systems/DebugRenderingSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

namespace DebugUtilities
{

	/*
	*	Given an index, returns a random color.
	*/
	Vector4 GetRandomColor(const uint64 index) NOEXCEPT
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

		return colors[index % colors.Size()];
	}

	/*
	*	Debug renders all axis aligned bounding boxes for grass vegetation.
	*/
	void DebugRenderGrassVegetationAxisAlignedBoundingBoxes() NOEXCEPT
	{
		const DynamicArray<GrassVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetGrassVegetationTypeInformations() };

		uint64 counter{ 0 };

		for (const GrassVegetationTypeInformation &information : *informations)
		{
			for (const VegetationPatchInformation &patchInformation : information._PatchInformations)
			{
				if (patchInformation._Valid)
				{
					DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData data;

					data._Box = patchInformation._AxisAlignedBoundingBox;
					data._Color = GetRandomColor(counter++);

					DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(data);
				}
			}
		}
	}

	/*
	*	Debug renders all axis aligned bounding boxes for solid vegetation.
	*/
	void DebugRenderSolidVegetationAxisAlignedBoundingBoxes() NOEXCEPT
	{
		const DynamicArray<SolidVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetSolidVegetationTypeInformations() };

		uint64 counter{ 0 };

		for (const SolidVegetationTypeInformation &information : *informations)
		{
			for (const VegetationPatchInformation &patchInformation : information._PatchInformations)
			{
				if (patchInformation._Valid)
				{
					DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData data;

					data._Box = patchInformation._AxisAlignedBoundingBox;
					data._Color = GetRandomColor(counter++);

					DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(data);
				}
			}
		}
	}

	/*
	*	Debug renders all axis aligned bounding boxes for terrain.
	*/
	void DebugRenderTerrainAxisAlignedBoundingBoxes() NOEXCEPT
	{
		uint8 counter{ 0 };

		const StaticArray<TerrainPatchInformation, 9> *const RESTRICT highDetailInformations{ TerrainSystem::Instance->GetHighDetailTerrainPatchInformations() };

		for (const TerrainPatchInformation &information : *highDetailInformations)
		{
			if (information._Valid)
			{
				DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData data;

				data._Box = information._AxisAlignedBoundingBox;
				data._Color = GetRandomColor(counter++);

				DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(data);
			}
		}

		const StaticArray<TerrainPatchInformation, 56> *const RESTRICT lowDetailInformations{ TerrainSystem::Instance->GetLowDetailTerrainPatchInformations() };

		for (const TerrainPatchInformation &information : *lowDetailInformations)
		{
			if (information._Valid)
			{
				DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData data;

				data._Box = information._AxisAlignedBoundingBox;
				data._Color = GetRandomColor(counter++);

				DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(data);
			}
		}
	}

}
#endif