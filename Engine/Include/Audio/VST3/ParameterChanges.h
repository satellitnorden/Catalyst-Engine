#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

//Third party.
#include <VST3/pluginterfaces/vst/ivstparameterchanges.h>

namespace VST3
{

	/*
	*	Implementation of VST3 param value queue.
	*/
	class ParamValueQueue final : public Steinberg::Vst::IParamValueQueue
	{

	public:

		/*
		*	Default constructor.
		*/
		FORCE_INLINE ParamValueQueue(const Steinberg::Vst::ParamID identifier) NOEXCEPT
			:
			_Identifier(identifier),
			_Value(0.0)
		{

		}

		VST3_REFERENCE_COUNTING();

		/*
		*	Queries an instance.
		*/
		FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void** obj) override
		{
			ASSERT(false, "Check if this ever happens!");
			return Steinberg::kNoInterface;
		}

		/*
		*	Returns the parameter ID.
		*/
		FORCE_INLINE Steinberg::Vst::ParamID PLUGIN_API getParameterId() override
		{
			return _Identifier;
		}

		/*
		*	Returns the number of points.
		*/
		FORCE_INLINE int32 PLUGIN_API getPointCount() override
		{
			return 1;
		}

		/*
		*	Returns the point.
		*/
		FORCE_INLINE Steinberg::tresult PLUGIN_API getPoint(int32 index, int32 &sampleOffset, Steinberg::Vst::ParamValue &value) override
		{
			sampleOffset = 0;
			value = _Value;

			return Steinberg::kResultOk;
		}

		
		/*
		*	Adds the point.
		*/
		FORCE_INLINE Steinberg::tresult PLUGIN_API addPoint(int32 sampleOffset, Steinberg::Vst::ParamValue value, int32 &index) override
		{
			_Value = value;

			index = 0;

			return Steinberg::kResultOk;
		}

	private:

		//The identifier.
		Steinberg::Vst::ParamID _Identifier;

		//The value.
		Steinberg::Vst::ParamValue _Value;

	};

	/*
	*	Implementation of VST3 parameter changes.
	*/
	class ParameterChanges final : public Steinberg::Vst::IParameterChanges
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
		*	Returns the number of parameters.
		*/
		FORCE_INLINE int32 PLUGIN_API getParameterCount() override
		{
			return static_cast<int32>(_ParamValueQueues.Size());
		}

		/*
		*	Returns the parameter data at the given index.
		*/
		FORCE_INLINE Steinberg::Vst::IParamValueQueue *PLUGIN_API getParameterData(int32 index) override
		{
			return &_ParamValueQueues[index];
		}

		/*
		*	Adds parameter data.
		*/
		FORCE_INLINE Steinberg::Vst::IParamValueQueue *PLUGIN_API addParameterData(const Steinberg::Vst::ParamID &id, int32 &index) override
		{
			_ParamValueQueues.Emplace(id);
			return &_ParamValueQueues.Back();
		}

		/*
		*	Clears these parameter changes.
		*/
		FORCE_INLINE void Clear() NOEXCEPT
		{
			_ParamValueQueues.Clear();
		}

	private:

		//The param value queues.
		DynamicArray<ParamValueQueue> _ParamValueQueues;

	};

}