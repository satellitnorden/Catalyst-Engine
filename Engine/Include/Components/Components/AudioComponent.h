#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/AudioAsset.h>

class AudioInitializationData final : public ComponentInitializationData
{

public:

	//The asset.
	AssetPointer<AudioAsset> _Asset;

};

class AudioInstanceData final
{

public:

	//The asset.
	AssetPointer<AudioAsset> _Asset;

};

class AudioComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		Audio,
		COMPONENT_INITIALIZE()
	);

};