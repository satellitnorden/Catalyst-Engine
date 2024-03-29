#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/Bone.h>

class Skeleton final
{

public:

	//The bones.
	DynamicArray<Bone> _Bones;

};