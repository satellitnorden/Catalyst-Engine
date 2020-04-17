#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class ShaderData final
{

public:

	//The stage.
	ShaderStage _Stage;

	//The data.
	DynamicArray<byte> _Data;

};