#pragma once

class Float16
{

public:

	//The data.
	union
	{
		//The individual components.
		struct
		{
			//The sign.
			uint16 _Sign : 1;

			//The exponent.
			uint16 _Exponent : 5;

			//The mantissa.
			uint16 _Mantissa : 10;
		} _IndividualComponents;
		
		//The data.
		uint16 _Data;
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Float16() NOEXCEPT
		:
		_Data(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE constexpr Float16(const Float16 &other) NOEXCEPT
		:
		_Data(other._Data)
	{

	}

	/*
	*	Conversion constructor.
	*/
	FORCE_INLINE constexpr Float16(const float32 other) NOEXCEPT
		:
		_Data(0)
	{
		*this = other;
	}

	/*
	*	Assignment by Float16 operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const Float16 &other) NOEXCEPT
	{
		_Data = other._Data;
	}

	/*
	*	Assignment by float32 operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const float32 &other) NOEXCEPT
	{
		//Split the float32 into individual components.
		struct
		{
			union
			{
				//The individual components.
				struct
				{
					//The sign.
					uint32 _Sign : 1;

					//The exponent.
					uint32 _Exponent : 8;

					//The mantissa.
					uint32 _Mantissa : 23;
				} _IndividualComponents;

				//The data.
				float32 _Data;
			};
		} float_components{ 0 };

		float_components._Data = other;

		//Copy the sign bit.
		_IndividualComponents._Sign = float_components._IndividualComponents._Sign;

		//Check for zero, denormal or too small value.
		if (float_components._IndividualComponents._Exponent <= 112)
		{
			//Set to zero.
			_IndividualComponents._Exponent = 0;
			_IndividualComponents._Mantissa = 0;

			//Exponent unbias the single, then bias the half precision.
			const int32 new_exponent{static_cast<int32>(float_components._IndividualComponents._Exponent - 127 + 15) };

			//Mantissa might be non-zero.
			if ((14 - new_exponent) <= 24)
			{
				//Calculate and assign the new mantissa.
				const uint32 new_mantissa{ static_cast<uint32>(float_components._IndividualComponents._Mantissa | 0x800000) };
				_IndividualComponents._Mantissa = new_mantissa >> (14 - new_exponent);

				//Determine if rounding should be performed.
				if ((new_mantissa >> (13 - new_exponent)) & 1)
				{
					++_Data;
				}
			}
		}

		//Check for Inf or NaN, or other invalid values.
		else if (float_components._IndividualComponents._Exponent >= 143)
		{
			//Set to maximum value.
			_IndividualComponents._Exponent = 30;
			_IndividualComponents._Mantissa = 1023;
		}

		//Otherwise, the incoming float32 value is valid.
		else
		{
			_IndividualComponents._Exponent = static_cast<int32>(float_components._IndividualComponents._Exponent) - 127 + 15;
			_IndividualComponents._Mantissa = static_cast<uint16>(float_components._IndividualComponents._Mantissa >> 13);
		}
	}

	/*
	*	float32 operator overload.
	*/
	FORCE_INLINE constexpr operator float32() const NOEXCEPT
	{
		//Split the float32 into individual components.
		struct
		{
			union
			{
				//The individual components.
				struct
				{
					//The sign.
					uint32 _Sign : 1;

					//The exponent.
					uint32 _Exponent : 8;

					//The mantissa.
					uint32 _Mantissa : 23;
				} _IndividualComponents;

				//The data.
				float32 _Data;
			};
		} result{ 0 };

		//Set the sign bit.
		result._IndividualComponents._Sign = _IndividualComponents._Sign;

		//Set the exponent and mantissa.
		if (_IndividualComponents._Exponent == 0)
		{
			const uint32 new_mantissa{ _IndividualComponents._Mantissa };

			if(new_mantissa == 0)
			{
				//Set to zero.
				result._IndividualComponents._Exponent = 0;
				result._IndividualComponents._Mantissa = 0;
			}
			else
			{
				//Unnormal number.
				const uint32 mantissa_shift{ 10 - static_cast<uint32>(static_cast<int32>(logf(static_cast<float32>(new_mantissa)) * (1.0f / logf(2.0f)))) };

				result._IndividualComponents._Exponent = 127 - (15 - 1) - mantissa_shift;
				result._IndividualComponents._Mantissa = new_mantissa << (mantissa_shift + 23 - 10);
			}
		}

		else if (_IndividualComponents._Exponent == 31)
		{
			//Invalid value, set to FLOAT16_MAXIMUM.
			result._IndividualComponents._Exponent = 142;
			result._IndividualComponents._Mantissa = 8380416;
		}

		else
		{
			//Normal number.
			result._IndividualComponents._Exponent = static_cast<int32>(_IndividualComponents._Exponent) - 15 + 127;
			result._IndividualComponents._Mantissa = static_cast<uint32>(_IndividualComponents._Mantissa) << 13;
		}

		return result._Data;
	}

};

//Type aliases.
using float16 = Float16;