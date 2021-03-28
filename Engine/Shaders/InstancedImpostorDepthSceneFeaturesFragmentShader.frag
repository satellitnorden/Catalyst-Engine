//Includes.
#include "CatalystMaterialCore.glsl"
#include "CatalystTransparency.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) float HALF_WIDTH;
    layout (offset = 20) float WHOLE_WIDTH;
    layout (offset = 24) float HEIGHT;
    layout (offset = 28) uint MATERIAL_INDEX;
    layout (offset = 32) float START_FADE_IN_DISTANCE;
    layout (offset = 36) float END_FADE_IN_DISTANCE;
    layout (offset = 40) float START_FADE_OUT_DISTANCE;
    layout (offset = 44) float END_FADE_OUT_DISTANCE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;
layout (location = 1) in float fragment_fade_opacity;

void CatalystShaderMain()
{
  //Retrieve the material.
  Material material = GLOBAL_MATERIALS[MATERIAL_INDEX];

  //Evaluate the material's opacity.
  float opacity;

  if (TEST_BIT(material._Properties, MATERIAL_PROPERTY_OPACITY_TEXTURE))
  {
    opacity = texture(sampler2D(GLOBAL_TEXTURES[material._Opacity], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).x;
  }

  else
  {
    opacity = UnpackColor(material._Opacity).x;
  }


  //Conditionally discard.
  if (opacity < 0.5f
      || ShouldClip(uint(gl_FragCoord.x), uint(gl_FragCoord.y), fragment_fade_opacity, true))
  {
    discard;
  }
}