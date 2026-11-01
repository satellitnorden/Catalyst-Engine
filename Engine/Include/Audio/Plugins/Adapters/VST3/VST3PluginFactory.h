#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>

//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3Component.h>
#include <Audio/Plugins/Adapters/VST3/VST3UUID.h>

//Third party.
#include <VST3/pluginterfaces/base/ipluginbase.h>
#include <VST3/pluginterfaces/vst/ivstaudioprocessor.h>

//Audio plugin.
#include <AudioPluginGlue.Generated.h>

class VST3PluginFactory final : public Steinberg::IPluginFactory
{

public:

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

		QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown_iid, VST3PluginFactory);
		QUERY_INTERFACE(_iid, obj, Steinberg::IPluginFactory_iid, VST3PluginFactory);

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
	*	Returns the factory info.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getFactoryInfo(Steinberg::PFactoryInfo *info) override
	{
		sprintf_s(info->vendor, "%s", AudioPluginGlue::GetVendor());
		info->url[0] = '\0';
		info->email[0] = '\0';
		info->flags = Steinberg::PFactoryInfo::FactoryFlags::kNoFlags;

		return Steinberg::kResultOk;
	}

	/*
	*	Returns the number of classes.
	*/
	FORCE_INLINE int32 PLUGIN_API countClasses() override
	{
		return 1;
	}

	/*
	*	Returns the class info.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getClassInfo(int32 index, Steinberg::PClassInfo *info) override
	{
		if (index != 0)
		{
			return Steinberg::kInvalidArgument;
		}

		GenerateClassID();

		Steinberg::copyTUID(info->cid, _ClassID);
		info->cardinality = Steinberg::PClassInfo::ClassCardinality::kManyInstances;
		sprintf_s(info->category, "%s", kVstAudioEffectClass);
		sprintf_s(info->name, "%s", AudioPluginGlue::GetName());

		return Steinberg::kResultOk;
	}

	/*
	*	Creates an instance of the given class.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API createInstance
	(
		Steinberg::FIDString cid,
		Steinberg::FIDString _iid,
		void **obj
	) override
	{
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		*obj = nullptr;

		GenerateClassID();

		if (!Steinberg::FUnknownPrivate::iidEqual(cid, _ClassID))
		{
			return Steinberg::kNoInterface;
		}

		if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Vst::IComponent_iid))
		{
			*obj = new VST3Component();

			return Steinberg::kResultOk;
		}

		return Steinberg::kNoInterface;
	}

private:

	//Denotes if we have the class ID.
	bool _HasClassID{ false };

	//The class ID.
	Steinberg::TUID _ClassID;

	/*
	*	Generates the class ID.
	*/
	FORCE_INLINE void GenerateClassID() NOEXCEPT
	{
		//Don't generate it twice.
		if (_HasClassID)
		{
			return;
		}

		//Generate the class ID.
		VST3UUID::Generate(AudioPluginGlue::GetIdentifier(), &_ClassID);

		//We now have a class ID!
		_HasClassID = true;
	}

};
#endif