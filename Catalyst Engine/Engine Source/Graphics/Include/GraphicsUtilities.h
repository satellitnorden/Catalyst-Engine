#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>
#include <Vector4.h>

namespace GraphicsUtilities
{

	/*
	*	Given 8 corners of a cube, determine if it is within the view frustum.
	*/
	bool IsCubeWithinViewFrustum(const Vector4 *CATALYST_RESTRICT corners) CATALYST_NOEXCEPT
	{
		float highestX{ -FLT_MAX };
		float lowestX{ FLT_MAX };
		float highestY{ -FLT_MAX };
		float lowestY{ FLT_MAX };
		float highestZ{ -FLT_MAX };
		float lowestZ{ FLT_MAX };

		for (uint8 i = 0; i < 8; ++i)
		{
			highestX = GameMath::Maximum(highestX, corners[i].X);
			lowestX = GameMath::Minimum(lowestX, corners[i].X);
			highestY = GameMath::Maximum(highestY, corners[i].Y);
			lowestY = GameMath::Minimum(lowestY, corners[i].Y);
			highestZ = GameMath::Maximum(highestZ, corners[i].Z);
			lowestZ = GameMath::Minimum(lowestZ, corners[i].Z);
		}

		if	(((highestX > 1.0f && lowestX > 1.0f) || (highestX < -1.0f && lowestX < -1.0f))
			||
			((highestY > 1.0f && lowestY > 1.0f) || (highestY < -1.0f && lowestY < -1.0f))
			||
			((highestZ > 1.0f && lowestZ > 1.0f) || (highestZ < 0.0f && lowestZ < 0.0f)))
		{
			return false;
		}

		else
		{
			return true;
		}
	}

}