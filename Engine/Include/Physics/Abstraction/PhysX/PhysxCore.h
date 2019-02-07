#pragma once

//Preprocessor defines.
#if !defined(CATALYST_CONFIGURATION_DEBUG)
	#define NDEBUG
#endif

//Core.
#include <Core/Essential/CatalystEssential.h>

//PhysX.
#include <ThirdParty/PhysX/PxPhysicsAPI.h>

/*
*	The filter shader.
*/
static physx::PxFilterFlags PhysxFilterShader(	physx::PxFilterObjectAttributes attributes0,
										physx::PxFilterData filterData0,
										physx::PxFilterObjectAttributes attributes1,
										physx::PxFilterData filterData1,
										physx::PxPairFlags& pairFlags,
										const void* constantBlock,
										physx::PxU32 constantBlockSize)
{
	return physx::PxFilterFlag::eDEFAULT;
}