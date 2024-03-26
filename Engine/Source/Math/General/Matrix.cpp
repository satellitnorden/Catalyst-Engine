//Header file.
#include <Math/General/Matrix.h>

//Math.
#include <Math/General/Quaternion.h>

/*
*	Constructor taking position, a rotation quaternion and scale as arguments.
*/
Matrix4x4::Matrix4x4(const Vector3<float32> &translation, const Quaternion &rotation, const Vector3<float32> &scale) NOEXCEPT
	:
	_Matrix{ { 1.0f, 0.0f, 0.0f, 0.0f },
			 { 0.0f, 1.0f, 0.0f, 0.0f },
			 { 0.0f, 0.0f, 1.0f, 0.0f },
			 { 0.0f, 0.0f, 0.0f, 1.0f } }
{
#if 1
	//First set the translation.
	_Matrix[3][0] = translation._X;
	_Matrix[3][1] = translation._Y;
	_Matrix[3][2] = translation._Z;

	//Set up rotation and scale simultaneously.
	const float32 X_2{ rotation._X + rotation._X };
	const float32 Y_2{ rotation._Y + rotation._Y };
	const float32 Z_2{ rotation._Z + rotation._Z };

	{
		const float32 xx2 = rotation._X * X_2;
		const float32 yy2 = rotation._Y * Y_2;
		const float32 zz2 = rotation._Z * Z_2;

		_Matrix[0][0] = (1.0f - (yy2 + zz2)) * scale._X;
		_Matrix[1][1] = (1.0f - (xx2 + zz2)) * scale._Y;
		_Matrix[2][2] = (1.0f - (xx2 + yy2)) * scale._Z;
	}

	{
		const float32 yz2 = rotation._Y * Z_2;
		const float32 wx2 = rotation._W * X_2;

		_Matrix[2][1] = (yz2 - wx2) * scale._Z;
		_Matrix[1][2] = (yz2 + wx2) * scale._Y;
	}

	{
		const float32 xy2 = rotation._X * Y_2;
		const float32 wz2 = rotation._W * Z_2;

		_Matrix[1][0] = (xy2 - wz2) * scale._Y;
		_Matrix[0][1] = (xy2 + wz2) * scale._X;
	}

	{
		const float32 xz2 = rotation._X * Z_2;
		const float32 wy2 = rotation._W * Y_2;

		_Matrix[2][0] = (xz2 + wy2) * scale._Z;
		_Matrix[0][2] = (xz2 - wy2) * scale._X;
	}

	_Matrix[0][3] = 0.0f;
	_Matrix[1][3] = 0.0f;
	_Matrix[2][3] = 0.0f;
	_Matrix[3][3] = 1.0f;
#else
	//Scale this matrix.
	Scale(scale);

	//Rotate the matrix.
	Rotate(rotation);

	//Translate this matrix.
	Translate(translation);
#endif
}

/*
*	Rotates this matrix.
*/
void Matrix4x4::Rotate(const Quaternion &rotation) NOEXCEPT
{
	Matrix4x4 rotation_matrix;

	const float32 qxx{ rotation._X * rotation._X };
	const float32 qyy{ rotation._Y * rotation._Y };
	const float32 qzz{ rotation._Z * rotation._Z };
	const float32 qxz{ rotation._X * rotation._Z };
	const float32 qxy{ rotation._X * rotation._Y };
	const float32 qyz{ rotation._Y * rotation._Z };
	const float32 qwx{ rotation._W * rotation._X };
	const float32 qwy{ rotation._W * rotation._Y };
	const float32 qwz{ rotation._W * rotation._Z };

	rotation_matrix._Matrix[0][0] = 1.0f - 2.0f * (qyy + qzz);
	rotation_matrix._Matrix[0][1] = 2.0f * (qxy + qwz);
	rotation_matrix._Matrix[0][2] = 2.0f * (qxz - qwy);

	rotation_matrix._Matrix[1][0] = 2.0f * (qxy - qwz);
	rotation_matrix._Matrix[1][1] = 1.0f - 2.0f * (qxx + qzz);
	rotation_matrix._Matrix[1][2] = 2.0f * (qyz + qwx);

	rotation_matrix._Matrix[2][0] = 2.0f * (qxz + qwy);
	rotation_matrix._Matrix[2][1] = 2.0f * (qyz - qwx);
	rotation_matrix._Matrix[2][2] = 1.0f - 2.0f * (qxx + qyy);

	*this = *this * rotation_matrix;

	Verify();
}