#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3PluginData.h>

//Third party.
#include <VST3/pluginterfaces/base/ustring.h>
#include <VST3/pluginterfaces/vst/ivsteditcontroller.h>
#include <VST3/pluginterfaces/vst/ivstunits.h>

class VST3EditController final : public Steinberg::Vst::IEditController
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VST3EditController(VST3PluginData *const RESTRICT plugin_data) NOEXCEPT
		:
		_PluginData(plugin_data)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~VST3EditController() NOEXCEPT
	{

	}

	/*
	*	Queries if this object has the given interface.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void **obj) override
	{
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		*obj = nullptr;

		QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown_iid, VST3EditController);
		QUERY_INTERFACE(_iid, obj, Steinberg::IPluginBase_iid, VST3EditController);
		QUERY_INTERFACE(_iid, obj, Steinberg::Vst::IEditController_iid, VST3EditController);

		return Steinberg::kNoInterface;
	}

	/*
	*	Adds a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API addRef() override
	{
		return 1;
	}

	/*
	*	Releases a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API release() override
	{
		return 1;
	}

	/*
	*	Initializes this edit controller.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API initialize(FUnknown *context) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Terminates this edit controller.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API terminate() override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Sets the component state.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream *state) override
	{
		//Nothing to do here. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Sets the state.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream *state) override
	{
		//Nothing to do here. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the state.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream *state) override
	{
		//Nothing to do here. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the number of parameters.
	*/
	FORCE_INLINE int32 PLUGIN_API getParameterCount() override
	{
		return _PluginData->_Plugin->GetNumberOfParameters();
	}

	/*
	*	Returns the parameter info.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getParameterInfo
	(
		int32 paramIndex,
		Steinberg::Vst::ParameterInfo &info
	) override
	{
		if (paramIndex >= _PluginData->_Plugin->GetNumberOfParameters())
		{
			return Steinberg::kInvalidArgument;
		}

		info.id = paramIndex;
		Steinberg::UString128(_PluginData->_Plugin->GetParameterName(paramIndex)).copyTo(info.title, 128);
		Steinberg::UString128(_PluginData->_Plugin->GetParameterName(paramIndex)).copyTo(info.shortTitle, 128);
		Steinberg::UString128(_PluginData->_Plugin->GetParameterPostFix(paramIndex)).copyTo(info.units, 128);
		info.stepCount = 0; //TODO!
		info.defaultNormalizedValue = _PluginData->_Plugin->GetParameterDefaultNormalized(paramIndex);
		info.unitId = Steinberg::Vst::kRootUnitId;
		info.flags = Steinberg::Vst::ParameterInfo::kCanAutomate;

		if (_PluginData->_Plugin->IsParameterEnumeration(paramIndex))
		{
			info.flags |= Steinberg::Vst::ParameterInfo::kIsList;
		}

		return Steinberg::kResultOk;
	}

	/*
	*	Returns the parameter string.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getParamStringByValue
	(
		Steinberg::Vst::ParamID id,
		Steinberg::Vst::ParamValue valueNormalized,
		Steinberg::Vst::String128 string
	) override
	{
		char buffer[64];

		if (!_PluginData->_Plugin->PrintParameterValueString(id, buffer, ARRAY_LENGTH(buffer), _PluginData->_Plugin->ParameterNormalizedToPlain(id, valueNormalized)))
		{
			return Steinberg::kInternalError;
		}

		Steinberg::UString128(buffer).copyTo(string, 128);

		return Steinberg::kResultOk;
	}

	/*
	*	Returns the parameter value.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getParamValueByString
	(
		Steinberg::Vst::ParamID id,
		Steinberg::Vst::TChar *string,
		Steinberg::Vst::ParamValue &valueNormalized
	) override
	{
		//Do I really need to implement this?
		return Steinberg::kNotImplemented;
	}

	/*
	*	Returns the plain parameter value, given a normalized one.
	*/
	FORCE_INLINE Steinberg::Vst::ParamValue PLUGIN_API normalizedParamToPlain
	(
		Steinberg::Vst::ParamID id,
		Steinberg::Vst::ParamValue valueNormalized
	) override
	{
		return static_cast<Steinberg::Vst::ParamValue>(_PluginData->_Plugin->ParameterNormalizedToPlain(id, static_cast<float32>(valueNormalized)));
	}

	/*
	*	Returns the normalized parameter value, given a plain one.
	*/
	FORCE_INLINE Steinberg::Vst::ParamValue PLUGIN_API plainParamToNormalized
	(
		Steinberg::Vst::ParamID id,
		Steinberg::Vst::ParamValue plainValue
	) override
	{
		return static_cast<Steinberg::Vst::ParamValue>(_PluginData->_Plugin->ParameterPlainToNormalized(id, static_cast<float32>(plainValue)));
	}

	/*
	*	Returns the parameters value, normalized.
	*/
	FORCE_INLINE Steinberg::Vst::ParamValue PLUGIN_API getParamNormalized(Steinberg::Vst::ParamID id) override
	{
		return static_cast<Steinberg::Vst::ParamValue>(_PluginData->_Plugin->GetParameterCurrentNormalized(id));
	}

	/*
	*	Sets the parameter value, normalized.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setParamNormalized(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue value) override
	{
		_PluginData->_Plugin->SetParameterNormalized(id, static_cast<float32>(value));

		return Steinberg::kResultOk;
	}

	/*
	*	Sets the component handler.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setComponentHandler(Steinberg::Vst::IComponentHandler *handler) override
	{
		//Don't think I need to care about this right now?
		return Steinberg::kResultOk;
	}

	/*
	*	Creates a view.
	*/
	FORCE_INLINE Steinberg::IPlugView *PLUGIN_API createView(Steinberg::FIDString name) override
	{
		//Not supported right now, I think.
		return nullptr;
	}

private:

	//The plugin data.
	VST3PluginData *RESTRICT _PluginData;

};
#endif