//Includes.
#include "CatalystMaterialCore.glsl"
#include "CatalystTransparency.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) uint MODEL_FLAGS;
    layout (offset = 20) float START_FADE_OUT_DISTANCE_SQUARED;
    layout (offset = 24) float END_FADE_OUT_DISTANCE_SQUARED;
    layout (offset = 28) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;
layout (location = 1) in float fragment_fade_out_opacity;

void CatalystShaderMain()
{
  //Retrieve the material.
	Material material = GLOBAL_MATERIALS[MATERIAL_INDEX];

	//Does this material have an opacity texture?
  float opacity = 1.0f;

  if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_TYPE_MASKED))
  {
    if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_OPACITY_TEXTURE))
    {
      opacity = texture(sampler2D(GLOBAL_TEXTURES[material._Opacity], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate)[0];
    }

    else
    {
      opacity = UnpackColor(material._Opacity)[0];
    }
  }

  //Discard based on fade out opacity.
  if ((fragment_fade_out_opacity < 1.0f && ShouldClip(uint(gl_FragCoord.x), uint(gl_FragCoord.y), fragment_fade_out_opacity, false))
      || opacity < 0.5f)
  {
    discard;
  }
}