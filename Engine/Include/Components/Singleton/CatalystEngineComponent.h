#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

class CatalystEngineComponent final
{

public:

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

	//Denotes whether or not the game should terminate.
	std::atomic<bool> _ShouldTerminate{ false };

	//Denotes the total frames.
	std::atomic<uint64> _TotalFrames{ 0 };

	//Denotes the delta time.
	float _DeltaTime{ 0.0f };

	//Denotes the total game time.
	float _TotalTime{ 0.0f };

};