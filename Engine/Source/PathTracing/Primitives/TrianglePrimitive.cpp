//Header file.
#include <PathTracing/Primitives/TrianglePrimitive.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	TrianglePrimitive::TrianglePrimitive(const PathTracing::Material *const RESTRICT material, const StaticArray<Vertex, 3> &vertices) NOEXCEPT
		:
		Primitive(material)
	{
		//Set up the triangle.
		_Triangle._Vertices[0] = vertices[0]._Position;
		_Triangle._Vertices[1] = vertices[1]._Position;
		_Triangle._Vertices[2] = vertices[2]._Position;

		//Set up the normals.
		_Normals[0] = vertices[0]._Normal;
		_Normals[1] = vertices[1]._Normal;
		_Normals[2] = vertices[2]._Normal;

		//Set up the tangents.
		_Tangents[0] = vertices[0]._Tangent;
		_Tangents[1] = vertices[1]._Tangent;
		_Tangents[2] = vertices[2]._Tangent;

		//Set up the texture coordinates.
		_TextureCoordinates[0] = vertices[0]._TextureCoordinate;
		_TextureCoordinates[1] = vertices[1]._TextureCoordinate;
		_TextureCoordinates[2] = vertices[2]._TextureCoordinate;
	}

	/*
	*	Returns the center of this primitive.
	*/
	NO_DISCARD Vector3<float32> TrianglePrimitive::Center() const NOEXCEPT
	{
		return (_Triangle._Vertices[0] + _Triangle._Vertices[1] + _Triangle._Vertices[2]) / 3.0f;
	}

	/*
	*	Returns the bounding box of this primitive.
	*/
	NO_DISCARD AxisAlignedBoundingBox3D TrianglePrimitive::BoundingBox() const NOEXCEPT
	{
		AxisAlignedBoundingBox3D bounding_box;

		for (const Vector3<float32> &vertex : _Triangle._Vertices)
		{
			bounding_box.Expand(vertex);
		}

		return bounding_box;
	}

	/*
	*	Traces a surface ray against this primitive.
	*	Should return if it hits, and if so, fill in the surface information.
	*/
	NO_DISCARD bool TrianglePrimitive::TraceSurface(const Ray &ray, PathTracing::SurfaceInformation *const RESTRICT surface_information) const NOEXCEPT
	{
		//Do the intersection.
		float32 hit_distance{ FLOAT32_MAXIMUM };

		if (CatalystGeometryMath::RayTriangleIntersection(ray, _Triangle, &hit_distance) && hit_distance < surface_information->_HitDistance)
		{
			//Calculate the hit position.
			const Vector3<float32> hit_position{ ray._Origin + ray._Direction * hit_distance };

			//Calculate the barycentric coordinates.
			const Vector3<float32> barycentric_coordinates{ CatalystGeometryMath::CalculateBarycentricCoordinates(_Triangle, hit_position) };

			//Fill in the surface information.
			surface_information->_HitDistance = hit_distance;
			surface_information->_HitPosition = ray._Origin + ray._Direction * hit_distance;
			surface_information->_GeometryNormal =	_Normals[0] * barycentric_coordinates[0]
													+ _Normals[1] * barycentric_coordinates[1]
													+ _Normals[2] * barycentric_coordinates[2];
			surface_information->_GeometryNormal.Normalize();
			surface_information->_Tangent = _Tangents[0] * barycentric_coordinates[0]
											+ _Tangents[1] * barycentric_coordinates[1]
											+ _Tangents[2] * barycentric_coordinates[2];
			surface_information->_Tangent.Normalize();
			surface_information->_TextureCoordinate =	_TextureCoordinates[0] * barycentric_coordinates[0]
														+ _TextureCoordinates[1] * barycentric_coordinates[1]
														+ _TextureCoordinates[2] * barycentric_coordinates[2];

			surface_information->FillInMaterialData(GetMaterial());

			//Return that the ray hit this triangle!
			return true;
		}

		else
		{
			//Return that the ray not hit this triangle.
			return false;
		}
	}

	/*
	*	Traces an occlusion ray against this primitive.
	*	Should return if it hits.
	*/
	NO_DISCARD bool TrianglePrimitive::TraceOcclusion(const Ray &ray) const NOEXCEPT
	{
		return CatalystGeometryMath::RayTriangleIntersection(ray, _Triangle);
	}

}