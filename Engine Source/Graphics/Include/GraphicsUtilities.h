#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>
#include <Vector4.h>

namespace GraphicsUtilities
{

	/*
	*	Given a resolution, generate plane vertices and indices.
	*/
	static void GeneratePlane(const uint64 resolution, DynamicArray<float> &vertices, DynamicArray<uint32> &indices) NOEXCEPT
	{
		vertices.Reserve(resolution * 5);
		indices.Reserve(resolution * resolution * 6);

		for (uint16 i = 0; i <= resolution; ++i)
		{
			for (uint16 j = 0; j <= resolution; ++j)
			{
				vertices.EmplaceUnsafe(-1.0f + (2.0f * static_cast<float>(i) / static_cast<float>(resolution)));
				vertices.EmplaceUnsafe(0.0f);
				vertices.EmplaceUnsafe(-1.0f + (2.0f * static_cast<float>(j) / static_cast<float>(resolution)));
				vertices.EmplaceUnsafe(static_cast<float>(i) / static_cast<float>(resolution));
				vertices.EmplaceUnsafe(static_cast<float>(j) / static_cast<float>(resolution));

				if (i != resolution && j != resolution)
				{
					indices.EmplaceUnsafe((i * (resolution + 1)) + j);
					indices.EmplaceUnsafe((i * (resolution + 1)) + j + 1);
					indices.EmplaceUnsafe(((i + 1) * (resolution + 1)) + j);

					indices.EmplaceUnsafe((i * (resolution + 1)) + j + 1);
					indices.EmplaceUnsafe(((i + 1) * (resolution + 1)) + j + 1);
					indices.EmplaceUnsafe(((i + 1) * (resolution + 1)) + j);
				}
			}
		}
	}

	/*
	*	Given 8 corners of a cube, determine if it is within the view frustum.
	*/
	static bool IsCubeWithinViewFrustum(const Vector4 *RESTRICT corners) NOEXCEPT
	{
		float highestX{ -FLOAT_MAXIMUM };
		float lowestX{ FLOAT_MAXIMUM };
		float highestY{ -FLOAT_MAXIMUM };
		float lowestY{ FLOAT_MAXIMUM };
		float highestZ{ -FLOAT_MAXIMUM };
		float lowestZ{ FLOAT_MAXIMUM };

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