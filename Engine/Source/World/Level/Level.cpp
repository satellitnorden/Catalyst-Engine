//Header file.
#include <World/Level/Level.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>
#include <Content/Assets/ModelAsset.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//World.
#include <World/Core/WorldTransform.h>

//Third party.
#include <ThirdParty/json/json.hpp>

/*
*	Serializes this level to the given file path.
*/
void Level::Serialize(const char *const RESTRICT file_path) NOEXCEPT
{
	//Set up the JSON object.
	nlohmann::json JSON;

	//Write all entities.
	nlohmann::json &entities{ JSON["Entities"] };

	for (const LevelEntry &level_entry : _LevelEntries)
	{
		EntitySerialization::SerializeToJSON(entities, level_entry._Name.Data(), level_entry._Entity);
	}

	//Writ the JSON to the file.
	std::ofstream file{ file_path };
	file << std::setw(4) << JSON;
	file.close();
}