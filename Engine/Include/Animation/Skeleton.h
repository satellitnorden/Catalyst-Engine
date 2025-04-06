#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/Bone.h>

class Skeleton final
{

public:

	//The root bone.
	Bone _RootBone;

	//The total number of bones.
	uint32 _TotalNumberOfBones;

};