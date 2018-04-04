#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class SoundBankData final
{

public:

	//The byte data.
	DynamicArray<byte> data;

	//The size of the sound bank data, denoted in bytes.
	uint64 size;

};