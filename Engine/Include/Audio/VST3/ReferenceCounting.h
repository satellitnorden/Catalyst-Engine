#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <VST3/pluginterfaces/base/funknown.h>

//STD.
#include <atomic>

//Adds VST3 reference counting. Put this inside your class.
#define VST3_REFERENCE_COUNTING()													\
	private:																		\
		std::atomic<uint32> _ReferenceCount{ 1 };									\
	public:																			\
		FORCE_INLINE Steinberg::uint32 addRef() override							\
		{																			\
			return ++_ReferenceCount;												\
		}																			\
		FORCE_INLINE Steinberg::uint32 release() override							\
		{																			\
			const uint32 previous_reference_count{ _ReferenceCount.fetch_sub(1) };	\
			ASSERT(previous_reference_count > 0, "Double release detected!");		\
			return previous_reference_count > 0 ? previous_reference_count - 1 : 0;	\
		}