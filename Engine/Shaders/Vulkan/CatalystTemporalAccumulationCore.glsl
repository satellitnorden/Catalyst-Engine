#if !defined(CATALYST_TEMPORAL_ACCUMULATION_CORE)
#define CATALYST_TEMPORAL_ACCUMULATION_CORE

//Accumulation description struct definition.
struct AccumulationDescription
{
	uint accumulations;
	uint instanceID;
	uint primitiveID;
};

/*
*	Layout:
*
*	12 bits: accumulations, max value 4095.
*	8 bits: instanceID, max value 255.
*	12 bits: primitiveID, max value 4095.
*/

/*
*	Packs the accumulation description.
*/
uint PackAccumulationDescription(AccumulationDescription accumulationDescription)
{
	return ((accumulationDescription.accumulations & 4095) << 20) | ((accumulationDescription.instanceID & 255) << 12) | (accumulationDescription.primitiveID & 4095);
}

/*
*	Unpacks the accumulation description.
*/
AccumulationDescription UnpackAccumulationDescription(uint packedAccumulationDescription)
{
	AccumulationDescription accumulationDescription;

	accumulationDescription.accumulations = (packedAccumulationDescription >> 20) & 4095;
	accumulationDescription.instanceID = (packedAccumulationDescription >> 12) & 255;
	accumulationDescription.primitiveID = packedAccumulationDescription & 4095;

	return accumulationDescription;
}

#endif