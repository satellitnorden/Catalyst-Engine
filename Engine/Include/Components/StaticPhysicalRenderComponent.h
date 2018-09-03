#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

class StaticPhysicalRenderComponent final
{

public:

	//Denotes whether or not this static physical model is in the view frustum.
	bool isInViewFrustum;

	//The model matrix.
	Matrix4 modelMatrix;

	//The render data table.
	RenderDataTableHandle renderDataTable{ };

	//The buffer.
	ConstantBufferHandle buffer{ };

	//The offset for the indices in the buffer.
	uint64 indexOffset{ 0 };

	//The index count.
	uint32 indexCount{ 0 };

};