#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysxCore.h>

class PhysxErrorCallback final : public physx::PxErrorCallback
{

public:

	/*
	*	The report error function.
	*/
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		ASSERT(false, message);
	}

};