#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>
#include <Core/General/HashString.h>
#include <Core/General/StaticString.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Plugin host base class, effects specializing in host specific plugin formats inherits from this.
*/
class PluginHost : public AudioEffect
{

public:

	/*
	*	Parameter class definition.
	*/
	class Parameter final
	{

	public:

		//The identifier.
		HashString _Identifier;

		//The name.
		StaticString<128> _Name;

		//The default normalized value.
		float64 _DefaultNormalizedValue;

		//The current normalized value.
		float64 _CurrentNormalizedValue;

		//The default plain value.
		float64 _DefaultPlainValue;

		//The current plain value.
		float64 _CurrentPlainValue;

		//The format identifier - Different formats (.vst3, .clap etc) stores identifiers differently, so support them all.
		union
		{
			uint32 _uint32; //VST3
		} _FormatIdentifier;

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~PluginHost() NOEXCEPT
	{

	}

	/*
	*	Initializes this plugin host. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool Initialize(const char* const RESTRICT plugin_file_path) NOEXCEPT = 0;

	/*
	*	Sets a parameter (normalized) with the given identifier. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool SetParameterNormalized(const HashString identifier, const float64 value) NOEXCEPT = 0;

	/*
	*	Sets a parameter (plain) with the given identifier. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool SetParameterPlain(const HashString identifier, const float64 value) NOEXCEPT = 0;

	/*
	*	Shows the UI. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool ShowUI() NOEXCEPT = 0;

	/*
	*	Hides the UI. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool HideUI() NOEXCEPT = 0;

	/*
	*	Returns the state. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool GetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT = 0;

	/*
	*	Sets the state. Returns if it succeeded.
	*/
	virtual NO_DISCARD bool SetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT = 0;

	/*
	*	Returns the plugin name.
	*/
	FORCE_INLINE NO_DISCARD const char *const RESTRICT GetPluginName() const NOEXCEPT
	{
		return _PluginName.Data();
	}

	/*
	*	Returns the parameters.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Parameter> &GetParameters() const NOEXCEPT
	{
		return _Parameters;
	}

protected:

	//Denotes if this plugin host is initialized.
	bool _Initialized{ false };

	//The plugin name.
	StaticString<64> _PluginName;

	//The parameters.
	DynamicArray<Parameter> _Parameters;

	/*
	*	Finds the parameter with the given identifier. Returns 'nullptr' if none was found.
	*/
	FORCE_INLINE NO_DISCARD const Parameter *const RESTRICT FindParameter(const HashString identifier) NOEXCEPT
	{
		for (const Parameter &parameter : _Parameters)
		{
			if (parameter._Identifier == identifier)
			{
				return &parameter;
			}
		}

		return nullptr;
	}

};