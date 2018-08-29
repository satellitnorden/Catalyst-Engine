#pragma once

//Core.
#include <Core/Containers/StaticArray.h>

template <uint64 Bytes>
class Padding final
{

private:

	//The underlying padding.
	StaticArray<byte, Bytes> padding;

	//Make sure the object actually is N bytes big.
	static_assert(sizeof(padding) == Bytes, "Padding object is not N bytes big as expected.");

};