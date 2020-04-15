#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/UserInterfaceMaterial.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>

class ImageUserInterfaceElement final : public UserInterfaceElement
{

public:

	//The material.
	UserInterfaceMaterial _Material;

};