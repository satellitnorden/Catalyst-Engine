//Header file.
#include <Rendering/Native/RenderingUtilities.h>

/*
*	Transforms an axis aligned bounding box from one space to another.
*/
void RenderingUtilities::TransformAxisAlignedBoundingBox(const AxisAlignedBoundingBox3D &original_box, const Matrix4x4 &transformation, AxisAlignedBoundingBox3D *const RESTRICT new_box) NOEXCEPT
{
	const Vector3<float> xMinimum{ transformation.GetRight() * original_box._Minimum._X };
	const Vector3<float> xMaximum{ transformation.GetRight() * original_box._Maximum._X };

	const Vector3<float> yMinimum{ transformation.GetUp() * original_box._Minimum._Y };
	const Vector3<float> yMaximum{ transformation.GetUp() * original_box._Maximum._Y };

	const Vector3<float> zMinimum{ transformation.GetForward() * original_box._Minimum._Z };
	const Vector3<float> zMaximum{ transformation.GetForward() * original_box._Maximum._Z };

	new_box->_Minimum = transformation.GetTranslation() + BaseMath::Minimum(xMinimum, xMaximum) + BaseMath::Minimum(yMinimum, yMaximum) + BaseMath::Minimum(zMinimum, zMaximum);
	new_box->_Maximum = transformation.GetTranslation() + BaseMath::Maximum(xMinimum, xMaximum) + BaseMath::Maximum(yMinimum, yMaximum) + BaseMath::Maximum(zMinimum, zMaximum);
}