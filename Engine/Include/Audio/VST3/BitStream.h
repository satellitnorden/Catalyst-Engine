#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

//Third party.
#include <VST3/pluginterfaces/base/ibstream.h>

namespace VST3
{

	/*
	*	Implementation of VST3 bit streams.
	*/
	class BitStream final : public Steinberg::IBStream
	{

	public:

		VST3_REFERENCE_COUNTING();

		/*
		*	Queries an instance.
		*/
		FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void **obj) override
		{
			ASSERT(false, "Check if this ever happens!");
			return Steinberg::kNoInterface;
		}

		/*
		*	Reads.
		*/
		FORCE_INLINE Steinberg::tresult read(void *buffer, int32 numBytes, int32 *numBytesRead) override
		{
			ASSERT(_StreamArchive != nullptr, "This bit stream needs a stream archive!");

			const uint64 number_of_available_bytes{ _StreamArchive->Size() - _StreamArchiveReadPosition };
			const uint64 number_of_bytes_to_read{ BaseMath::Minimum<uint64>(numBytes, number_of_available_bytes) };

			_StreamArchive->Read(buffer, number_of_bytes_to_read, &_StreamArchiveReadPosition);

			if (numBytesRead)
			{
				*numBytesRead = number_of_bytes_to_read;
			}

			return Steinberg::kResultOk;
		}

		/*
		*	Writes.
		*/
		FORCE_INLINE Steinberg::tresult write(void *buffer, int32 numBytes, int32 *numBytesWritten) override
		{
			ASSERT(_StreamArchive != nullptr, "This bit stream needs a stream archive!");

			_StreamArchive->Write(buffer, numBytes);

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
				case Steinberg::IBStream::IStreamSeekMode::kIBSeekSet:
				{
					_StreamArchiveReadPosition = static_cast<uint64>(pos);
					
					if (result)
					{
						*result = static_cast<int64>(_StreamArchiveReadPosition);
					}

					return Steinberg::kResultOk;
				}

				default:
				{
					ASSERT(false, "Implement case!");

					return Steinberg::kInternalError;
				}
			}
		}

		/*
		*	Telld.
		*/
		FORCE_INLINE Steinberg::tresult tell(int64 *pos) override
		{
			*pos = _StreamArchiveReadPosition;

			return Steinberg::kResultOk;
		}

		/*
		*	Sets the stream archive.
		*/
		FORCE_INLINE void SetStreamArchive(StreamArchive *const RESTRICT stream_archive) NOEXCEPT
		{
			_StreamArchive = stream_archive;
		}

	private:

		//The stream archive.
		StreamArchive *RESTRICT _StreamArchive{ nullptr };

		//The stream archive read position.
		uint64 _StreamArchiveReadPosition{ 0 };

	};

}