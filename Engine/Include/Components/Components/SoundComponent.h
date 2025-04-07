#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/SoundAsset.h>

class SoundInitializationData final : public ComponentInitializationData
{

public:

	//The sound.
	AssetPointer<SoundAsset> _Sound;

};

class SoundInstanceData final
{

public:

	//The sound.
	AssetPointer<SoundAsset> _Sound;

};

class SoundComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		Sound,
		COMPONENT_INITIALIZE()
	);

};