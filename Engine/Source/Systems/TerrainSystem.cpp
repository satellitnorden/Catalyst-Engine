//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Terrain.
#include <Terrain/TerrainUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainSystem);

/*
*	Initializes the terrain system.
*/
void TerrainSystem::InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	//Copy over the relevant properties.
	_Properties._Height = configuration._Height;
	_Properties._TextureTilingFactor = configuration._TextureTilingFactor;
	_Properties._PatchResolution = configuration._PatchResolution;
	_Properties._PatchSize = configuration._PatchSize;
}

/*
*	Adds a terrain patch.
*/
void TerrainSystem::AddTerrainPatch(TerrainPatchInformation &&information) NOEXCEPT
{
	//Set up the new terrain patch.
	_Patches.EmplaceSlow();
	TerrainPatch *const RESTRICT patch{ &_Patches.Back() };
	patch->_X = information._X;
	patch->_Y = information._Y;

	//Generate the normal for the normal/depth map.
	patch->_NormalHeightMap = std::move(information._NormalHeightMap);
	TerrainUtilities::GeneratePatchNormals(_Properties._PatchSize, _Properties._PatchResolution, _Properties._Height, &patch->_NormalHeightMap);

	//Set up the new terrain render information.
	_RenderInformation.EmplaceSlow();
	TerrainRenderInformation *const RESTRICT renderInformation{ &_RenderInformation.Back() };

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainUtilities::GenerateTerrainPlane(	_Properties._PatchResolution,
											Vector3(0.0f, 0.0f, 0.0f),
											_Properties._PatchSize,
											patch->_NormalHeightMap,
											information._LayerWeightsMap,
											_Properties._Height,
											_Properties._TextureTilingFactor,
											&vertices,
											&indices);

	StaticArray<void *RESTRICT, 2> bufferData;

	bufferData[0] = vertices.Data();
	bufferData[1] = indices.Data();

	StaticArray<uint64, 2> bufferDataSizes;

	bufferDataSizes[0] = sizeof(TerrainVertex) * vertices.Size();
	bufferDataSizes[1] = sizeof(uint32) * indices.Size();

	renderInformation->_Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	renderInformation->_IndexOffset = bufferDataSizes[0];
	renderInformation->_IndexCount = static_cast<uint32>(indices.Size());
	renderInformation->_RenderDataTable = information._Material._RenderDataTable;
	renderInformation->_DisplacementInformation = information._DisplacementInformation;
}