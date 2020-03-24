#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class TopLevelAccelerationStructureInstanceData final
{

public:

	//The transform.
	Matrix4x4 _Transform{ MatrixConstants::IDENTITY };

	//The bottom level acceleration structure.
	AccelerationStructureHandle _BottomLevelAccelerationStructure{ EMPTY_HANDLE };

	//The hit group index.
	uint32 _HitGroupIndex{ UINT32_MAXIMUM };

	//The instance index.
	uint32 _InstanceIndex{ UINT32_MAXIMUM };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr TopLevelAccelerationStructureInstanceData() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr TopLevelAccelerationStructureInstanceData(	const Matrix4x4 &initial_transform,
																		const AccelerationStructureHandle &initial_bottom_level_acceleration_structure,
																		const uint32 initial_hit_group_index,
																		const uint32 initial_instance_index) NOEXCEPT
		:
		_Transform(initial_transform),
		_BottomLevelAccelerationStructure(initial_bottom_level_acceleration_structure),
		_HitGroupIndex(initial_hit_group_index),
		_InstanceIndex(initial_instance_index)
	{

	}

};