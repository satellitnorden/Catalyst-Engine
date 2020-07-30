#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

/*
*	Process for adding a new sound mix component:
*
*	1. Add an entry into the SOUND_MIX_COMPONENTS list below.
*	2. Add a class called the entry + "SoundMixComponent". Use the existing ones as blueprints. MUST contain the "State" class as well as the "Construct", "Process" and "Destruct" functions, everything else is optional.
*	3. Add a "Create" function in the SoundMixComponent class. Use the existings ones as blueprints.
*/

//List of all available sound mix components.
#define SOUND_MIX_COMPONENTS	SOUND_MIX_COMPONENT(AutomaticGainCorrection)			\
								SOUND_MIX_COMPONENT(Limiter)

/*
*	This sound mix component will automatically correct the gain of incoming samples to never exceed 1.0f.
*/
class AutomaticGainCorrectionSoundMixComponent final
{

public:

	/*
	*	State class definition.
	*/
	class State final
	{

	public:

		//The gain.
		float32 _Gain;

	};

	/*
	*	The construct function.
	*/
	FORCE_INLINE static void Construct() NOEXCEPT
	{

	}

	/*
	*	The process function.
	*/
	FORCE_INLINE static void Process(State *const RESTRICT state, float32 *const RESTRICT sample) NOEXCEPT
	{
		//Update the gain.
		state->_Gain -= CatalystBaseMath::Maximum(CatalystBaseMath::Absolute(*sample * state->_Gain) - 1.0f, 0.0f);

		//Process the sample.
		*sample *= state->_Gain;
	}

	/*
	*	The destruct function.
	*/
	FORCE_INLINE static void Destruct(State *const RESTRICT state) NOEXCEPT
	{
#if !defined(CATALYST_CONFIGURATION_FINAL)
		PRINT_TO_OUTPUT("Automatic gain correction's gain value: " << state->_Gain << ".");
#endif
	}

};

/*
*	This sound mix component will clamp incoming samples between -CEILING and CEILING.
*/
class LimiterSoundMixComponent final
{

public:

	/*
	*	State class definition.
	*/
	class State final
	{

	public:

		//The boost
		float32 _Boost;

		//The ceiling
		float32 _Ceiling;

	};

	/*
	*	The construct function.
	*/
	FORCE_INLINE static void Construct() NOEXCEPT
	{

	}

	/*
	*	The process function.
	*/
	FORCE_INLINE static void Process(State *const RESTRICT state, float32 *const RESTRICT sample) NOEXCEPT
	{
		//Apply the clamp.
		*sample = CatalystBaseMath::Clamp(*sample * state->_Boost, -state->_Ceiling, state->_Ceiling);
	}

	/*
	*	The destruct function.
	*/
	FORCE_INLINE static void Destruct(State *const RESTRICT state) NOEXCEPT
	{

	}

};

/*
*	Base sound mix component.
*/
class SoundMixComponent final
{

public:

	//Enumerations covering all types.
	enum class Type : uint8
	{
#define SOUND_MIX_COMPONENT(VALUE) VALUE,
		SOUND_MIX_COMPONENTS
#undef SOUND_MIX_COMPONENT
	};

	/*
	*	Creates an automatic gain correction sound mix component.
	*/
	FORCE_INLINE static NO_DISCARD SoundMixComponent CreateAutomaticGainCorrection(const float32 initial_gain) NOEXCEPT
	{
		SoundMixComponent new_component{ Type::AutomaticGainCorrection };

		new_component._AutomaticGainCorrectionState._Gain = initial_gain;

		return new_component;
	}

	/*
	*	Creates a limiter sound mix component.
	*/
	FORCE_INLINE static NO_DISCARD SoundMixComponent CreateLimiter(const float32 initial_boost, const float32 initial_ceiling) NOEXCEPT
	{
		SoundMixComponent new_component{ Type::Limiter };

		new_component._LimiterState._Boost = initial_boost;
		new_component._LimiterState._Ceiling = initial_ceiling;

		return new_component;
	}

	/*
	*	Default constructor.
	*/
	FORCE_INLINE SoundMixComponent() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the type as argument.
	*/
	FORCE_INLINE SoundMixComponent(const Type initial_type) NOEXCEPT
		:
		_Type(initial_type)
	{
		switch (_Type)
		{
#define SOUND_MIX_COMPONENT(VALUE)	case Type:: ## VALUE ## :						\
									{												\
										VALUE ## SoundMixComponent::Construct();	\
																					\
										break;										\
									}

			SOUND_MIX_COMPONENTS

#undef SOUND_MIX_COMPONENT

		default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~SoundMixComponent() NOEXCEPT
	{
		switch (_Type)
		{
#define SOUND_MIX_COMPONENT(VALUE)	case Type:: ## VALUE ## :										\
									{																\
										VALUE ## SoundMixComponent::Destruct(&_ ## VALUE ## State);	\
																									\
										break;														\
									}

			SOUND_MIX_COMPONENTS

#undef SOUND_MIX_COMPONENT

		default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	The process function.
	*/
	FORCE_INLINE void Process(float32 *const RESTRICT sample) NOEXCEPT
	{
		//Different processing depending on type.
		switch (_Type)
		{
#define SOUND_MIX_COMPONENT(VALUE)	case Type:: ## VALUE ## :												\
									{																		\
										VALUE ## SoundMixComponent::Process(&_ ## VALUE ## State, sample);	\
																											\
										break;																\
									}

			SOUND_MIX_COMPONENTS

#undef SOUND_MIX_COMPONENT

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

private:

	//The type.
	Type _Type;

	//The state.
	union
	{
#define SOUND_MIX_COMPONENT(VALUE) VALUE ## SoundMixComponent::State _ ## VALUE ## State;
		SOUND_MIX_COMPONENTS
#undef SOUND_MIX_COMPONENT
	};

};