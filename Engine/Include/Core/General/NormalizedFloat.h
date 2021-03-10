#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename UNDERLYING_TYPE>
class NormalizedFloat final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr NormalizedFloat() NOEXCEPT
		:
		_Data(static_cast<UNDERLYING_TYPE>(0))
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE constexpr NormalizedFloat(const NormalizedFloat &other) NOEXCEPT
		:
		_Data(other._Data)
	{

	}

	/*
	*	Conversion from float32 constructor.
	*/
	FORCE_INLINE constexpr NormalizedFloat(const float32 other) NOEXCEPT
		:
		_Data(static_cast<UNDERLYING_TYPE>(other * static_cast<float32>(std::numeric_limits<UNDERLYING_TYPE>::max())))
	{
		
	}

	/*
	*	Conversion from float64 constructor.
	*/
	FORCE_INLINE constexpr NormalizedFloat(const float64 other) NOEXCEPT
		:
		_Data(static_cast<UNDERLYING_TYPE>(other * static_cast<float64>(std::numeric_limits<UNDERLYING_TYPE>::max())))
	{

	}

	/*
	*	float32 operator overload.
	*/
	FORCE_INLINE constexpr operator float32() const NOEXCEPT
	{
		return static_cast<float32>(_Data) / static_cast<float32>(std::numeric_limits<UNDERLYING_TYPE>::max());
	}

	/*
	*	float64 operator overload.
	*/
	FORCE_INLINE constexpr operator float64() const NOEXCEPT
	{
		return static_cast<float64>(_Data) / static_cast<float64>(std::numeric_limits<UNDERLYING_TYPE>::max());
	}

private:

	//The underlying data.
	UNDERLYING_TYPE _Data;

};

//Type aliases.
using NormalizedFloat8 = NormalizedFloat<uint8>;
using NormalizedFloat16 = NormalizedFloat<uint16>;
using NormalizedFloat32 = NormalizedFloat<uint32>;
using NormalizedFloat64 = NormalizedFloat<uint64>;