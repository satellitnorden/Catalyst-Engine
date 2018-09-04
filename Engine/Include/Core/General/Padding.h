#pragma once

//Core.
#include <Core/Containers/StaticArray.h>

template <uint64 BYTES>
class Padding final
{

private:

	//The underlying padding.
	StaticArray<byte, BYTES> _Padding;

	//Make sure the object actually is N bytes big.
	static_assert(sizeof(_Padding) == BYTES, "Padding object is not N bytes big as expected.");

};