//Header file.
#include <Systems/VegetationSystem.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(VegetationSystem);

/*
*	Updates the vegetation system synchronously during the closing update phase.
*/
void VegetationSystem::OpeningUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{

}

/*
*	Adds a vegetation type.
*/
void VegetationSystem::AddVegetationType(const VegetationAddTypeInformation &addInformation) NOEXCEPT
{
	//Create the new vegetation information.
	_VegetationTypeInformations.EmplaceSlow();
	VegetationTypeInformation *const RESTRICT information{ &_VegetationTypeInformations.Back() };

	//Just copy the properties and the model.
	information->_Properties = addInformation._Properties;
	information->_Model = addInformation._Model;

	//Fill in the patch and the patch render informations.
	for (uint8 i = 0; i < 9; ++i)
	{
		information->_PatchInformations[i]._X = 0;
		information->_PatchInformations[i]._Y = 0;

		information->_PatchRenderInformations[i]._Draw = true;

		AxisAlignedBoundingBox box;
		box._Minimum = Vector3(-100.0f, 0.0f, -100.0f);
		box._Maximum = Vector3(100.0f, 0.0f, 100.0f);
		DynamicArray<Matrix4> transformations;

		for (uint32 i = 0; i < information->_Properties._Density; ++i)
		{
			Matrix4 newTransformation;

			if (information->_Properties._PlacementFunction(box, &newTransformation))
			{
				transformations.EmplaceSlow(newTransformation);
			}
		}

		RenderingUtilities::CreateTransformationsBuffer(transformations, &information->_PatchRenderInformations[i]._TransformationsBuffer);
		information->_PatchRenderInformations[i]._NumberOfTransformations = static_cast<uint32>(transformations.Size());
	}
}