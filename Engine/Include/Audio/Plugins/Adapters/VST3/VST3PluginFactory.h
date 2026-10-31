#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>

//Third party.
#include <uuid/uuid.h>
#include <VST3/pluginterfaces/base/ipluginbase.h>

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

		return Steinberg::kNoInterface;
	}

	/*
	*	Adds a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API addRef() override
	{
		return ++_NumberOfReferences;
	}

	/*
	*	Releases a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API release() override
	{
		return --_NumberOfReferences;
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

		switch (AudioPluginGlue::GetMainCategory())
		{
			case Audio::Plugin::MainCategory::INSTRUMENT:
			{
				sprintf_s(info->category, "Instrument");

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		sprintf_s(info->name, AudioPluginGlue::GetName());

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
		return Steinberg::kNotImplemented;
	}

private:

	//The number of references.
	uint32 _NumberOfReferences{ 1 };

	//Denotes if we have the class ID.
	bool _HasClassID{ false };

	//The class ID.
	Steinberg::TUID _ClassID;

	/*
	*	Generates the class ID.
	*/
	FORCE_INLINE void GenerateClassID() NOEXCEPT
	{
		//This defines the namespace - It was generated once with UUID v4, and should stay constant _forever_.
		constexpr uuid_t NAMESPACE
		{
			68,		221,	94,		127,
			244,	144,	72,		71,
			163,	29,		228,	215,
			57,		4,		31,		233
		};

		//Don't generate it twice.
		if (_HasClassID)
		{
			return;
		}

		//Generate the UUID.
		uuid_t UUID;

		uuid_v5(&NAMESPACE, AudioPluginGlue::GetIdentifier(), StringUtilities::StringLength(AudioPluginGlue::GetIdentifier()), &UUID);

		//Copy it over to the class ID.
		Memory::Copy(_ClassID, &UUID, sizeof(Steinberg::TUID));

		//We now have a class ID!
		_HasClassID = true;
	}

};
#endif