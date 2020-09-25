#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/UserInterfaceMaterial.h>

//User interface.
#include <UserInterface/UserInterfaceElementDescription.h>

class ImageUserInterfaceElementDescription final : public UserInterfaceElementDescription
{

public:

	//The material.
	UserInterfaceMaterial _Material;

};