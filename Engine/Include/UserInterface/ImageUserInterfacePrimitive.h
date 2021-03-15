#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/UserInterfaceMaterial.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>

class ImageUserInterfacePrimitive final : public UserInterfacePrimitive
{

public:

	//The material.
	UserInterfaceMaterial _Material;

};