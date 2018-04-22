//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    layout (offset = 0) float cameraFieldOfViewCosine; //Offset; 0 - Size; 16
    layout (offset = 16) mat4 inverseCameraMatrix; //Offset; 16 - Size; 64
    layout (offset = 80) mat4 inverseProjectionMatrix; //Offset; 80 - Size; 64
    layout (offset = 144) mat4 originViewMatrix; //Offset; 144 - Size; 64
    layout (offset = 208) mat4 viewMatrix; //Offset; 208 - Size; 64
    layout (offset = 272) vec3 cameraForwardVector; //Offset; 272 - Size; 16
    layout (offset = 288) vec3 cameraWorldPosition; //Offset; 288 - Size; 16

    //Directional light data.
    layout (offset = 304) float directionalLightIntensity; //Offset; 304 - Size; 16
    layout (offset = 320) mat4 directionalLightViewMatrix; //Offset; 320 - Size; 64
    layout (offset = 384) vec3 directionalLightDirection; //Offset; 384 - Size; 16
    layout (offset = 400) vec3 directionalLightColor; //Offset; 400 - Size; 16
    layout (offset = 416) vec3 directionalLightScreenSpacePosition; //Offset; 416 - Size; 16

    //Environment data.
    layout (offset = 432) float environmentBlend; //Offset; 432 - Size; 4

    //General data.
    layout (offset = 436) float deltaTime; //Offset; 436 - Size; 4
    layout (offset = 440) float totalGameTime; //Offset; 440 - Size; 4

    //Point light data.
    layout (offset = 444) int numberOfPointLights; //Offset; 444 - Size; 4
    layout (offset = 448) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 448 - Size; 128
    layout (offset = 576) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 576 - Size; 128
    layout (offset = 704) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 704 - Size; 128
    layout (offset = 832) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 832 - Size; 128

    //Spot light data.
    layout (offset = 960) int numberOfSpotLights; //Offset; 960 - Size; 16
    layout (offset = 976) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 976 - Size; 128
    layout (offset = 1104) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1104 - Size; 128
    layout (offset = 1232) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1232 - Size; 128
    layout (offset = 1360) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1360 - Size; 128
    layout (offset = 1488) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1488 - Size; 128
    layout (offset = 1616) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1616 - Size; 128
    layout (offset = 1744) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1744 - Size; 128

    //Physics data.
    layout (offset = 1872) float windStrength; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//The vegetation properties
layout (std140, set = 1, binding = 1) uniform VegetationProperties
{
    float cutoffDistance;
};

//The quad vertices.
vec3 quadVertices[4] = vec3[]
(
	vec3(-0.5f, 0.0f, 0.0f),
	vec3(-0.5f, 1.0f, 0.0f),
	vec3(0.5f, 0.0f, 0.0f),
	vec3(0.5f, 1.0f, 0.0f)
);

//The quad texture coordinates.
vec2 quadTextureCoordinates[4] = vec2[]
(
	vec2(0.0f, 1.0f),
	vec2(0.0f, 0.0f),
	vec2(1.0f, 1.0f),
	vec2(1.0f, 0.0f)
);

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//In parameters.
layout (location = 0) in vec3 vertexPosition[];
layout (location = 1) in vec2 vertexScale[];
layout (location = 2) in float vertexRotation[];

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;
layout (location = 1) out float fragmentCosineRotation;
layout (location = 2) out float fragmentSineRotation;

//Globals.
float cosineRotation;
float sineRotation;

/*
*	Returns the squared length of a vector.
*/
float SquaredLength(vec3 vector)
{
	return (vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z);
}

/*
*	Given a seed, return a random float.
*/
float RandomFloat(vec2 seed)
{
    return fract(sin(dot(seed.xy, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

/*
*	Returns the wind modulator for a given position.
*/
vec3 GetWindModulator(vec3 position)
{
	float windCosineModulator = (cos(totalGameTime * windStrength * (1.0f + RandomFloat(vec2(position.x, position.y)))) * 0.5f + 0.5f) * 0.5f + 0.8f;
	float windSinusModulator = (sin((position.x * windDirection.x) + (position.z * windDirection.z) + totalGameTime * windStrength) * 0.5f + 1.25f) * windCosineModulator;
	return windDirection * (windStrength * 0.01f) * windSinusModulator;
}

/*
*	Calculates the rotation.
*/
void CalculateRotation()
{
	cosineRotation = cos(vertexRotation[0]);
	sineRotation = sin(vertexRotation[0]);
}

/*
*	Given an index, constructs one bottom vertex of the quad.
*/
void ConstructBottomVertex(int index)
{
	//Start with the base vertex.
	vec3 vertex = quadVertices[index];
	float vertexHeight = vertex.y;

	//Apply the scale.
	vec2 vertexScale = vertexScale[0];
	vertex *= vec3(vertexScale.x, vertexScale.y, 1.0f);

	//Apply the rotation.
	float tempX = vertex.x * cosineRotation + vertex.z * sineRotation;
	vertex.z = -vertex.x * sineRotation + vertex.z * cosineRotation;
	vertex.x = tempX;

	//Apply the position.
	vertex += vertexPosition[0];

	//Calculate the final screen space position.
	gl_Position = viewMatrix * vec4(vertex, 1.0f);

	//Set the fragment properties.
	fragmentTextureCoordinate = quadTextureCoordinates[index];
	fragmentCosineRotation = cosineRotation;
	fragmentSineRotation = sineRotation;

	EmitVertex();
}

/*
*	Given an index, constructs one top vertex of the quad.
*/
void ConstructTopVertex(int index)
{
	//Start with the base vertex.
	vec3 vertex = quadVertices[index];

	//Apply the scale.
	vec2 vertexScale = vertexScale[0];
	vertex *= vec3(vertexScale.x, vertexScale.y, 1.0f);

	//Apply the rotation.
	float tempX = vertex.x * cosineRotation + vertex.z * sineRotation;
	vertex.z = -vertex.x * sineRotation + vertex.z * cosineRotation;
	vertex.x = tempX;

	//Apply the position.
	vertex += vertexPosition[0];

	//Apply the wind modulator.
	vec3 windModulator = GetWindModulator(vertex);
	vertex += windModulator;

	//Calculate the final screen space position.
	gl_Position = viewMatrix * vec4(vertex, 1.0f);

	//Set the fragment properties.
	fragmentTextureCoordinate = quadTextureCoordinates[index];
	fragmentCosineRotation = cosineRotation;
	fragmentSineRotation = sineRotation;

	EmitVertex();
}

void main()
{
	//Determine the length to the camera.
	float distanceToCamera = SquaredLength(vertexPosition[0] - cameraWorldPosition);

	if (distanceToCamera < cutoffDistance)
	{
		//Construct the quad.
		CalculateRotation();
	   	ConstructBottomVertex(0);
	   	ConstructTopVertex(1);
	   	ConstructBottomVertex(2);
	   	ConstructTopVertex(3);

	   	EndPrimitive();
	}
}