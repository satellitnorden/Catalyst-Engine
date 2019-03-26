//Header file.
#include <Resources/Creation/ResourceCreator.h>

/*
*	Creates a model.
*/
void ResourceCreator::CreateModel(ModelData *const RESTRICT data, Model *const RESTRICT model) NOEXCEPT
{
	//Just... Copy. (:
	model->_Vertices = std::move(data->_Vertices);
	model->_Indices = std::move(data->_Indices);
	model->_Extent = std::move(data->_Extent);
}