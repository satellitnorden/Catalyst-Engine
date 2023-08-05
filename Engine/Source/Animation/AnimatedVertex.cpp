//Header file.
#include <Animation/AnimatedVertex.h>

/*
*	Transforms this animated vertex.
*/
void AnimatedVertex::Transform(const Matrix4x4 &transformation, const float texture_coordinate_rotation) NOEXCEPT
{
	//Transform the position.
	const Vector4<float32> transformed_position{ transformation * Vector4<float32>(_Position, 1.0f) };

	_Position._X = transformed_position._X;
	_Position._Y = transformed_position._Y;
	_Position._Z = transformed_position._Z;

	//Transform the normal.
	const Vector4<float32> transformed_normal{ transformation * Vector4<float32>(_Normal, 0.0f) };

	_Normal._X = transformed_normal._X;
	_Normal._Y = transformed_normal._Y;
	_Normal._Z = transformed_normal._Z;

	//Transform the tangent.
	const Vector4<float32> transformed_tangent{ transformation * Vector4<float32>(_Tangent, 0.0f) };

	_Tangent._X = transformed_tangent._X;
	_Tangent._Y = transformed_tangent._Y;
	_Tangent._Z = transformed_tangent._Z;

	//Rotate the texture coordinate.
	_TextureCoordinate.Rotate(texture_coordinate_rotation);
}