#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

class StaticPhysicalRenderComponent final
{

public:

	//The model matrix.
	Matrix4 _ModelMatrix;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The offset for the indices in the buffer.
	uint64 _IndexOffset{ 0 };

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The index count.
	uint32 _IndexCount{ 0 };

	//Denotes whether or not this static physical model is in the view frustum.
	bool _IsInViewFrustum;

};