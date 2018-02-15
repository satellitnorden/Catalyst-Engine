#pragma once

//Engine core.
#include <EngineCore.h>

class VulkanRenderingSystem final
{

public:

	//System declaration.
	DECLARE_SYSTEM(VulkanRenderingSystem);

	/*
	*	Default constructor.
	*/
	VulkanRenderingSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VulkanRenderingSystem() NOEXCEPT;

};