#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox2D.h>

//Sound.
#include <Sound/MIDIMessage.h>

/*
*	This is a base class for audio plugins, meant to represent some generic processing unit that can be reused in different contexts.
*/
class AudioPlugin
{

public:

	/*
	*	Initialize parameters class definition.
	*/
	class InitializeParameters final
	{

	public:

		//The directory that this plugin operates in.
		DynamicString _Directory;

		//The sample rate.
		float32 _SampleRate;

	};

	/*
	*	Parameter class definition.
	*/
	class Parameter final
	{

	public:

		//Enumeration covering all formats.
		enum class Format : uint8
		{
			GAIN,
			PERCENT,
			MILLISECONDS
		};

		//The name.
		const char *RESTRICT _Name;

		//The identifier.
		HashString _Identifier;

		//The index.
		uint64 _Index;

		//The data.
		float32 *RESTRICT _Data;

		//The minimum.
		float32 _Minimum;

		//The maximum.
		float32 _Maximum;

		//The step.
		float64 _Step;

		//The format.
		Format _Format;

	};

	/*
	*	Control class definition.
	*/
	class Control final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			KNOB
		};

		//The bounding box.
		AxisAlignedBoundingBox2D _BoundingBox;

		//The type.
		Type _Type;

		//The parameter.
		HashString _Parameter;

	};

	/*
	*	Control tab class definition.
	*/
	class ControlTab final
	{

	public:

		//The name.
		const char *RESTRICT _Name;

		//The controls.
		DynamicArray<Control> _Controls;

	};

	/*
	*	Control layout class definition.
	*/
	class ControlLayout final
	{

	public:

		//The tabs.
		DynamicArray<ControlTab> _Tabs;

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~AudioPlugin() NOEXCEPT
	{

	}

	/*
	*	Returns the latency, in seconds.
	*/
	FORCE_INLINE virtual NO_DISCARD float32 GetLatency() NOEXCEPT
	{
		return 0.0f;
	}

	/*
	*	Returns the parameters.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Parameter> &GetParameters() const NOEXCEPT
	{
		return _Parameters;
	}

	/*
	*	Returns the control layout.
	*/
	FORCE_INLINE NO_DISCARD const ControlLayout* const RESTRICT GetControlLayout() const NOEXCEPT
	{
		return &_ControlLayout;
	}

	/*
	*	Initializes this audio plugin.
	*/
	FORCE_INLINE virtual void Initialize(const InitializeParameters &parameters) NOEXCEPT
	{
		//Store the initialize parameters.
		_InitializeParameters = parameters;
	}

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const float32 value) NOEXCEPT
	{
		if (_InitializeParameters._SampleRate != value)
		{
			_InitializeParameters._SampleRate = value;
			OnSampleRateChanged();
		}
	}

	/*
	*	Callback for this audio plugin to process the given buffer.
	*/
	FORCE_INLINE virtual void Process(const float32 input, float32 *const RESTRICT output, const uint8 channel_index) NOEXCEPT
	{
		//Default to passthrough.
		*output = input;
	}

	/*
	*	Advances this audio plugin.
	*/
	FORCE_INLINE virtual void Advance() NOEXCEPT
	{

	}

	/*
	*	Callback for when this plugin receives MIDI messages.
	*/
	FORCE_INLINE virtual void OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT
	{

	}

protected:

	//The initialize parameters.
	InitializeParameters _InitializeParameters;

	//The parameters.
	DynamicArray<Parameter> _Parameters;

	//The control layout.
	ControlLayout _ControlLayout;

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE virtual void OnSampleRateChanged() NOEXCEPT
	{

	}

};