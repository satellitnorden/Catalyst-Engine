#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/UserInterfaceElementDescription.h>

class ImageUserInterfaceElementDescription final : public UserInterfaceElementDescription
{

public:

	//The image texture index.
	uint32 _ImageTextureIndex;

};