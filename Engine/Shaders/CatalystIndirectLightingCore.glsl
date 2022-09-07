#if !defined(CATALYST_INDIRECT_LIGHTING_CORE)
#define CATALYST_INDIRECT_LIGHTING_CORE

/*
*	Screen space indirect lighting data struct definition.
*/
struct ScreenSpaceIndirectLightingData
{
	//The hit position.
	vec3 _HitPosition;

	//The hit percentage.
	float _HitPercentage;
};

/*
*	Packs ScreenSpaceIndirectLightingData into a vec4.
*/
vec4 PackScreenSpaceIndirectLightingData(ScreenSpaceIndirectLightingData data)
{
	vec4 output_data;

	//Pack the hit position.
	output_data[0] = data._HitPosition.x;
	output_data[1] = data._HitPosition.y;
	output_data[2] = data._HitPosition.z;

	//Pack the hit percentage.
	output_data[3] = data._HitPercentage;

	return output_data;
}

/*
*	Unpacks vec4 into ScreenSpaceIndirectLightingData.
*/
ScreenSpaceIndirectLightingData PackScreenSpaceIndirectLightingData(vec4 data)
{
	ScreenSpaceIndirectLightingData output_data;

	//Unpack the hit position.
	output_data._HitPosition.x = data[0];
	output_data._HitPosition.y = data[1];
	output_data._HitPosition.z = data[2];

	//Unpack the hit percentage.
	output_data._HitPercentage = data[3];

	return output_data;
}
#endif