//Header file.
#include <Resources/Creation/ResourceCreator.h>

/*
*	Creates a model.
*/
void ResourceCreator::CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT
{
	//Create the axis-aligned bounding box.
	model->_AxisAlignedBoundingBox._Minimum = Vector3<float>(-data->_Extent, -data->_Extent, -data->_Extent);
	model->_AxisAlignedBoundingBox._Maximum = Vector3<float>(data->_Extent, data->_Extent, data->_Extent);

	//Just... Copy the rest. (:
	model->_Vertices = std::move(data->_Vertices);
	model->_Indices = std::move(data->_Indices);
}