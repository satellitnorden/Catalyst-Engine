#pragma once

namespace MultiThreadingUtilities
{

	/*
	*	Since many compare-and-swap functions expect a reference for the expected value, here's some generic ones that can be passed in.
	*/
	static bool expectedFalse{ false };
	static bool expectedTrue{ true };

}