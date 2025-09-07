#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

//STD.
#include <cmath>

/*
*	Class implementing an approximation of a triode.
*	Triodes are often used in the preamp stage of tube amps.
*/
class Triode final
{

public:

	//Enumeration covering all types.
	enum class Type : uint32
	{
		/*
		*	High gain with lots of crunch when pushed, but can still do cleans. Almost universal in preamp stages.
		*/
		_12AX7,

		/*
		*	Medium gain, a bit cleaner and brighter.
		*/
		_12AT7,

		/*
		*	Low gain, clean and warm.
		*/
		_12AU7,

		NUMBER_OF_TYPES
	};

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		//The amplification factor.
		float32 _AmplificationFactor{ 100.0f };

		//The plate constant.
		float32 _PlateConstant{ 600.0f };

		//The grid constant.
		float32 _GridConstant{ 1'060.0f };

		//The plate voltage.
		float32 _PlateVoltage{ 250.0f };

		//The grid bias.
		float32 _GridBias{ -1.5f };

		//The voltage scale.
		float32 _VoltageScale{ 200.0f };

	};

	/*
	*	Returns the parameters given the type.
	*/
	FORCE_INLINE static void GetParameters(const Type type, Parameters *const RESTRICT parameters) NOEXCEPT
	{
		switch (type)
		{
			case Type::_12AX7:
			{
				parameters->_AmplificationFactor = 100.0f;
				parameters->_PlateConstant = 600.0f;
				parameters->_GridConstant = 1'060.0f;
				parameters->_PlateVoltage = 250.0f;
				parameters->_GridBias = -1.5f;
				parameters->_VoltageScale = 200.0f;

				break;
			}

			case Type::_12AT7:
			{
				parameters->_AmplificationFactor = 60.0f; 
				parameters->_PlateConstant = 550.0f;
				parameters->_GridConstant = 1000.0f;
				parameters->_PlateVoltage = 250.0f;
				parameters->_GridBias = -1.5f;
				parameters->_VoltageScale = 200.0f;

				break;
			}

			case Type::_12AU7:
			{
				parameters->_AmplificationFactor = 20.0f;
				parameters->_PlateConstant = 500.0f;
				parameters->_GridConstant = 900.0f;
				parameters->_PlateVoltage = 250.0f;
				parameters->_GridBias = -2.0f;
				parameters->_VoltageScale = 200.0f;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Initializes this triode.
	*/
	FORCE_INLINE void Initialize(const Parameters &parameters, const float32 output_scale = 0.5f) NOEXCEPT
	{
		//Copy the parameters.
		_Parameters = parameters;

		//Calculate the idle current.
		_IdleCurrent = CalculatePlateCurrent(_Parameters._GridBias);

		//Numerically calculate small-signal transconductance around bias.
		const float32 positive_plate_current{ CalculatePlateCurrent(_Parameters._GridBias + 0.001f) };
		const float32 negative_plate_current{ CalculatePlateCurrent(_Parameters._GridBias - 0.001f) };
		const float32 gain{ (positive_plate_current - negative_plate_current) / (2.0f * 0.001f) };

		//Calculate the output scale.
		const float32 denominator{ gain * _Parameters._VoltageScale };
		_OutputScale = denominator > 0.0f ? 1.0f / denominator : 0.0f;

		//Attenuate slightly so it doesn't get too hot.
		_OutputScale *= output_scale;
	}

	/*
	*	Processes a sample.
	*/
	FORCE_INLINE NO_DISCARD float32 Process(const float32 input_sample) NOEXCEPT
	{
		//Calculate the input voltage.
		const float32 input_voltage{ input_sample * _Parameters._VoltageScale + _Parameters._GridBias };

		//Calculate the plate current.
		float32 plate_current{ CalculatePlateCurrent(input_voltage) };

		//Remove the DC offset.
		plate_current -= _IdleCurrent;

		//Scale by the output scale.
		plate_current *= _OutputScale;

		//Saturate the output.
		return std::tanh(plate_current);
	}

private:

	//The parameters.
	Parameters _Parameters;

	//The idle current.
	float32 _IdleCurrent{ 0.0f };

	//The output scale.
	float32 _OutputScale{ 1.0f };

	/*
	*	Calculates the plate current.
	*/
	FORCE_INLINE NO_DISCARD float32 CalculatePlateCurrent(const float32 input) NOEXCEPT
	{
		const float64 term_1{ static_cast<float64>(input) + static_cast<float64>(_Parameters._PlateVoltage) / static_cast<float64>(_Parameters._AmplificationFactor) };
		const float64 exponent_input{ static_cast<float64>(_Parameters._GridConstant) * term_1 / static_cast<float64>(_Parameters._PlateConstant) };
		
		/*
		*	Clamp between [-700, 700] as we're approaching the limits of 'std::exp', otherwise it returns infinity.
		*	Values above/below this aren't musically relevant anyway.
		*/
		const float64 exponent{ std::exp(BaseMath::Clamp<float64>(exponent_input, -700.0, 700.0)) };
		const float64 log_term{ std::log1p(exponent) };

		return static_cast<float32>((static_cast<float64>(_Parameters._PlateConstant) / static_cast<float64>(_Parameters._GridConstant)) * (log_term * log_term));
	}

};