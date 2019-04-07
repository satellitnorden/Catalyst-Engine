#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class VulkanGeometryInstance final
{

public:

	//The transform.
	float _Transform[12];

	//The instance custom index.
	uint32 _InstanceCustomIndex : 24;

	//The mash.
	uint32 _Mask : 8;

	//The instance offset.
	uint32 _InstanceOffset : 24;
	
	//The flags.
	uint32 _Flags : 8;

	//The bottom level acceleration structure handle.
	uint64 _BottomLevelAccelerationStructureHandle;

};

static_assert(sizeof(VulkanGeometryInstance) == 64, "Oh no...");