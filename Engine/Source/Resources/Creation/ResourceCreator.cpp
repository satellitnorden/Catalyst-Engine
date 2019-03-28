//Header file.
#include <Resources/Creation/ResourceCreator.h>

/*
*	Creates a model.
*/
void ResourceCreator::CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT
{
	//Just... Copy. (:
	model->_AxisAlignedBoundingBox = std::move(data->_AxisAlignedBoundingBox);
	model->_Vertices = std::move(data->_Vertices);
	model->_Indices = std::move(data->_Indices);
}

/*
*	Creates a texture cube.
*/
void ResourceCreator::CreateTextureCube(TextureCubeData *const RESTRICT data, TextureCube<Vector4<byte>> *const RESTRICT texture) NOEXCEPT
{
	//Uhm.
}

/*
*	Creates a texture 2D.
*/
void ResourceCreator::CreateTexture2D(Texture2DData *const RESTRICT data, Texture2D<Vector4<byte>> *const RESTRICT texture) NOEXCEPT
{
	//Do something. Please. ):
	texture->Initialize(data->_Width, data->_Height, reinterpret_cast<const Vector4<byte> *const RESTRICT>(data->_Data[0].Data()));
}