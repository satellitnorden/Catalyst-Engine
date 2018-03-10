//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//In parameters
layout (location = 0) in vec3 fragmentPosition;

//Texture samplers.
layout (binding = 0) uniform sampler2D equirectangularTexture;

//Out parameters
layout (location = 0) out vec4 fragmentColor;

//Globals.
vec2 inverseAtan = vec2(0.1591f, 0.3183f);

/*
*	Given a 3D position, return a spherical texture coordinate.
*/
vec2 CalculateSphericalTextureCoordinate(vec3 position)
{
    vec2 textureCoordinate = vec2(atan(position.z, position.x), asin(position.y));

    textureCoordinate *= inverseAtan;
    textureCoordinate += 0.5f;

    return textureCoordinate;
}

void main()
{	
	//Get the spherical texture coordinate.
    vec2 textureCoordinate = CalculateSphericalTextureCoordinate(normalize(fragmentPosition));

    //The resulting color is just the color at the texture coordinates.
    fragmentColor = texture(equirectangularTexture, textureCoordinate);
}