#pragma once

//Core.
#include <Core/Containers/StaticArray.h>

template <uint64 Bytes>
class Padding final
{

private:

	//The underlying padding.
	StaticArray<byte, Bytes> padding;

};