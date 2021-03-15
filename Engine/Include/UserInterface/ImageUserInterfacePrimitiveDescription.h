#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/UserInterfaceMaterial.h>

//User interface.
#include <UserInterface/UserInterfacePrimitiveDescription.h>

class ImageUserInterfacePrimitiveDescription final : public UserInterfacePrimitiveDescription
{

public:

	//The material.
	UserInterfaceMaterial _Material;

};