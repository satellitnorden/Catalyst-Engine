#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Utilities/ResourceBuildingUtilities.h>

//File.
#include <File/Core/File.h>
#include <File/Readers/FBXReader.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Systems.
#include <Systems/ResourceSystem.h>

/*
*	Calculates the average color of the texture at the given file path.
*/
NO_DISCARD Vector4<float32> ResourceBuildingUtilities::CalculateAverageColor(const char *const RESTRICT texture_file_path) NOEXCEPT
{
	//Read the texture.
	Texture2D<Vector4<float32>> texture;

	switch (File::GetExtension(texture_file_path))
	{
		case File::Extension::JPG:
		{
			if (!JPGReader::Read(texture_file_path, &texture))
			{
				ASSERT(false, "Couldn't read %s", texture_file_path);
			}

			break;
		}

		case File::Extension::PNG:
		{
			if (!PNGReader::Read(texture_file_path, &texture))
			{
				ASSERT(false, "Couldn't read %s", texture_file_path);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Calculate the average color.
	const float32 dimensions_reciprocal{ 1.0f / (texture.GetWidth() * texture.GetHeight()) };
	Vector4<float32> average_color{ 0.0f, 0.0f, 0.0f, 0.0f };

	for (uint32 Y{ 0 }; Y < texture.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < texture.GetWidth(); ++X)
		{
			average_color += texture.At(X, Y) * dimensions_reciprocal;
		}
	}

	//Return the average color.
	return average_color;
}
#endif