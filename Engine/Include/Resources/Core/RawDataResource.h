#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/Resource.h>

class RawDataResource final : public Resource
{

public:

	//The data.
	DynamicArray<byte> _Data;


};