#if !defined(CATALYST_SHADOW_UTILITIES)
#define CATALYST_SHADOW_UTILITIES

//Preprocessor defines.
#define MAXIMUM_WIND_SPEED (30.0f)
#define INVERSE_MAXIMUM_WIND_SPEED (0.033333f)
#define TREE_VEGETATION_BASE_WIND_AFFECTION (0.1f)
#define WIND_SPEED_TIME_MULTIPLIER 4.0f

/*
*   Given a world position and normal, calculates the wind modulator for grass vegetation.
*/
vec3 CalculateWindModulator(vec3 position, vec3 normal)
{
	//Calculate the wind speed multiplier.
	float windSpeedMultiplier = windSpeed * INVERSE_MAXIMUM_WIND_SPEED;

	//Calculate the extended wind speed multiplier.
	float extendedWindSpeedMultiplier = 1.0f + windSpeedMultiplier;

	//Calculate the time factor.
	float timeFactor = totalTime * (1.0f + (windSpeedMultiplier * WIND_SPEED_TIME_MULTIPLIER));

    //Large scale motion.
    float largeScaleX = windDirection.x * extendedWindSpeedMultiplier * (sin(position.x + position.y + position.z + timeFactor) + 1.0f);
    float largeScaleZ = windDirection.z * extendedWindSpeedMultiplier * (sin(position.x + position.y + position.z + timeFactor) + 1.0f);

    //Medium scale motion.
    float mediumScaleX = windDirection.x + EULERS_NUMBER * sin(PHI * (position.x + position.y + position.z + timeFactor));
    float mediumScaleZ = windDirection.z + PHI * sin(EULERS_NUMBER * (position.x + position.y + position.z + timeFactor));

    //Small scale motion.
    vec3 smallScale = vec3(	INVERSE_PI * 8.0f * sin(PI * (position.x + position.y + position.z + timeFactor)),
    						INVERSE_PI * 2.0f * sin(EULERS_NUMBER * (position.x + position.y + position.z + timeFactor)),
    						INVERSE_PI * 4.0f * sin(DOUBLE_PI * (position.x + position.y + position.z + timeFactor))) * normal;

    return vec3(largeScaleX + mediumScaleX + smallScale.x, smallScale.y, largeScaleZ + mediumScaleZ + smallScale.z) * windSpeedMultiplier;
}

/*
*   Given an instance, calculates the wind modulator for tree vegetation.
*/
vec3 CalculateTreeVegetationWindModulator(vec3 position)
{
    //Calculate the wind speed multiplier.
    float windSpeedMultiplier = windSpeed * INVERSE_MAXIMUM_WIND_SPEED;

    //Calculate the extended wind speed multiplier.
    float extendedWindSpeedMultiplier = 1.0f + windSpeedMultiplier;

    //Calculate the time factor.
    float timeFactor = totalTime * (1.0f + (windSpeedMultiplier * WIND_SPEED_TIME_MULTIPLIER));

    //Large scale motion.
    float largeScaleX = windDirection.x * EULERS_NUMBER * extendedWindSpeedMultiplier * (sin((position.x + position.y + position.z + timeFactor) * SQUARE_ROOT_OF_TWO) + 1.0f);
    float largeScaleZ = windDirection.z * PI * extendedWindSpeedMultiplier * (sin((position.x + position.y + position.z + timeFactor) * PHI) + 1.0f);

    //Medium scale motion.
    float mediumScaleX = windDirection.x * SQUARE_ROOT_OF_TWO * sin((position.x + position.y + position.z + timeFactor) * EULERS_NUMBER);
    float mediumScaleZ = windDirection.z * PHI * sin((position.x + position.y + position.z + timeFactor) * PI);

    return vec3(largeScaleX + mediumScaleX, 0.0f, largeScaleZ + mediumScaleZ) * windSpeedMultiplier * TREE_VEGETATION_BASE_WIND_AFFECTION;
}
#endif