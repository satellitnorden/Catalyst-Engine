#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <VST3/pluginterfaces/base/ibstream.h>

//STD.
#include <atomic>

/*
*	Implementation of VST3 bit streams.
*/
class BitStream final : public Steinberg::IBStream
{

public:

	/*
	*	Queries an instance.
	*/
	FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void **obj) override
	{
		ASSERT(false, "Check if this ever happens!");
		return Steinberg::kNoInterface;
	}

	/*
	*	Queries an instance.
	*/
	FORCE_INLINE Steinberg::uint32 addRef() override
	{
		return ++_ReferenceCount;
	}

	/*
	*	Queries an instance.
	*/
	FORCE_INLINE Steinberg::uint32 release() override
	{
		const uint32 previous_reference_count{ _ReferenceCount.fetch_sub(1) };

		ASSERT(previous_reference_count > 0, "Double release detected!");

		return previous_reference_count > 0 ? previous_reference_count - 1 : 0;
	}

	/*
	*	Reads.
	*/
	FORCE_INLINE Steinberg::tresult read(void *buffer, int32 numBytes, int32 *numBytesRead) override
	{
		ASSERT((_CurrentReadIndex + numBytes) <= _Buffer.Size(), "Reading out of bounds!");
		Memory::Copy(buffer, &_Buffer[_CurrentReadIndex], numBytes);
		_CurrentReadIndex += numBytes;
		
		if (numBytesRead)
		{
			*numBytesRead = numBytes;
		}

		return Steinberg::kResultOk;
	}

	/*
	*	Writes.
	*/
	FORCE_INLINE Steinberg::tresult write(void *buffer, int32 numBytes, int32 *numBytesWritten) override
	{
		for (int32 byte_index{ 0 }; byte_index < numBytes; ++byte_index)
		{
			_Buffer.Emplace(static_cast<const byte *const RESTRICT>(buffer)[byte_index]);
		}

		if (numBytesWritten)
		{
			*numBytesWritten = numBytes;
		}

		return Steinberg::kResultOk;
	}

	/*
	*	Seeks.
	*/
	FORCE_INLINE Steinberg::tresult seek(int64 pos, int32 mode, int64 *result) override
	{
		switch (mode)
		{
			default:
			{
				ASSERT(false, "Implement case!");

				break;
			}
		}

		return Steinberg::kResultOk;
	}

	/*
	*	Telld.
	*/
	FORCE_INLINE Steinberg::tresult tell(int64 *pos) override
	{
		*pos = _CurrentReadIndex;

		return Steinberg::kResultOk;
	}

private:

	//The reference count.
	std::atomic<uint32> _ReferenceCount{ 1 };

	//The buffer.
	DynamicArray<byte> _Buffer;

	//The current write pointer.
	uint64 _CurrentReadIndex{ 0 };

};