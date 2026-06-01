#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

//Third party.
#include <VST3/pluginterfaces/vst/ivsteditcontroller.h>

namespace VST3
{

	class ComponentHandler final : public Steinberg::Vst::IComponentHandler
	{

	public:

		VST3_REFERENCE_COUNTING();

		/*
		*	Queries an instance.
		*/
		FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void** obj) override
		{
			//Check that we have valid arguments.
			if (!obj)
			{
				return Steinberg::kInvalidArgument;
			}

			//Reset the object.
			*obj = nullptr;

			//Are they asking for an unknown?
			if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::FUnknown_iid))
			{
				*obj = static_cast<Steinberg::FUnknown*>(this);
			}

			//Are they asking for a component handler?
			else if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Vst::IComponentHandler_iid))
			{
				*obj = static_cast<Steinberg::Vst::IComponentHandler*>(this);
			}

			//Add a reference if we set the object.
			if (*obj)
			{
				addRef();
				return Steinberg::kResultOk;
			}

			//Otherwise, return that we don't have this interface.
			else
			{
				return Steinberg::kNoInterface;
			}
		}

		/*
		*	Begins an edit.
		*/
		FORCE_INLINE Steinberg::tresult beginEdit(Steinberg::Vst::ParamID id) override
		{
			return Steinberg::kResultOk;
		}

		/*
		*	Performs an edit.
		*/
		FORCE_INLINE Steinberg::tresult performEdit(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized) override
		{
			if (_Parameters)
			{
				for (PluginHost::Parameter &parameter : (*_Parameters))
				{
					if (parameter._FormatIdentifier._uint32 == id)
					{
						parameter._CurrentNormalizedValue = valueNormalized;
						parameter._CurrentPlainValue = _EditController->normalizedParamToPlain(id, valueNormalized);

						break;
					}
				}
			}

			return Steinberg::kResultOk;
		}

		/*
		*	Ends an edit.
		*/
		FORCE_INLINE Steinberg::tresult endEdit(Steinberg::Vst::ParamID id) override
		{
			return Steinberg::kResultOk;
		}

		/*
		*	Restarts the component.
		*/
		FORCE_INLINE Steinberg::tresult restartComponent(Steinberg::int32 flags) override
		{
			switch (flags)
			{
				case Steinberg::Vst::RestartFlags::kLatencyChanged:
				{
					//This is fine, we're not dealing with latency right now.
					return Steinberg::kResultOk;
				}

				default:
				{
					ASSERT(false, "Invalid case!");
					return Steinberg::kInternalError;
				}
			}
		}

		/*
		*	Sets the edit controller.
		*/
		FORCE_INLINE void SetEditController(Steinberg::Vst::IEditController *const RESTRICT edit_controller) NOEXCEPT
		{
			_EditController = edit_controller;
		}

		/*
		*	Sets the parameters.
		*/
		FORCE_INLINE void SetParameters(DynamicArray<PluginHost::Parameter> *const RESTRICT parameters) NOEXCEPT
		{
			_Parameters = parameters;
		}

	private:

		//The edit controller.
		Steinberg::Vst::IEditController *RESTRICT _EditController{ nullptr };

		//The parameters.
		DynamicArray<PluginHost::Parameter> *RESTRICT _Parameters{ nullptr };

	};

}