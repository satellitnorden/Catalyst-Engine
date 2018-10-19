#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

class ClairvoyantPlayer final : public Updateable
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantPlayer);

	/*
	*	Default constructor.
	*/
	ClairvoyantPlayer() NOEXCEPT { }

	/*
	*	Initializes the Clairvoyant player.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the Clairvoyant player asynchronously during the logic update phase.
	*/
	bool LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

private:

	/*
	*	Applies gamepad controls.
	*/
	void ApplyGamepadControls(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Applies keyboard controls.
	*/
	void ApplyKeyboardControls(const UpdateContext *const RESTRICT context) NOEXCEPT;

};