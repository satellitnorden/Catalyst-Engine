#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/AntiAliasingRenderPass.h>
#include <Rendering/Engine/RenderPasses/AboveOceanRenderPass.h>
#include <Rendering/Engine/RenderPasses/BelowOceanRenderPass.h>
#include <Rendering/Engine/RenderPasses/BloomHorizontalRenderPass.h>
#include <Rendering/Engine/RenderPasses/BloomVerticalRenderPass.h>
#if !defined(CATALYST_FINAL)
#include <Rendering/Engine/RenderPasses/DebugAxisAlignedBoundingBoxRenderPass.h>
#include <Rendering/Engine/RenderPasses/DebugScreenBoxRenderPass.h>
#endif
#include <Rendering/Engine/RenderPasses/DepthOfFieldHorizontalRenderPass.h>
#include <Rendering/Engine/RenderPasses/DepthOfFieldVerticalRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalDynamicPhysicalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalShadowHorizontalBlurRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalShadowVerticalBlurRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalSolidVegetationShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalTerrainShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalTreeVegetationTrunkRenderPass.h>
#include <Rendering/Engine/RenderPasses/DynamicOutlineRenderPass.h>
#include <Rendering/Engine/RenderPasses/DynamicPhysicalRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailDebrisVegetationRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailGrassVegetationColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailGrassVegetationDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailSolidVegetationRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailTreeVegetationCrownColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailTreeVegetationCrownDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailTreeVegetationTrunkRenderPass.h>
#include <Rendering/Engine/RenderPasses/LightingRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailDebrisVegetationRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailGrassVegetationColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailGrassVegetationDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailSolidVegetationRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailTreeVegetationCrownColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailTreeVegetationCrownDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/LowDetailTreeVegetationTrunkRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailDebrisVegetationRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailGrassVegetationColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailGrassVegetationDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailSolidVegetationRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailTreeVegetationCrownColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailTreeVegetationCrownDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/MediumDetailTreeVegetationTrunkRenderPass.h>
#include <Rendering/Engine/RenderPasses/ParticleSystemRenderPass.h>
#include <Rendering/Engine/RenderPasses/SkyRenderPass.h>
#include <Rendering/Engine/RenderPasses/TerrainColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/TerrainDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/ToneMappingRenderPass.h>
#include <Rendering/Engine/RenderPasses/TreeVegetationImpostorColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/TreeVegetationImpostorDepthRenderPass.h>
#include <Rendering/Engine/RenderPasses/VolumetricFogRenderPass.h>