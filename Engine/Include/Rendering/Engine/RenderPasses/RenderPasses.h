#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#if !defined(CATALYST_FINAL)
#include <Rendering/Engine/RenderPasses/DebugAxisAlignedBoundingBoxRenderPass.h>
#include <Rendering/Engine/RenderPasses/DebugScreenBoxRenderPass.h>
#endif
#include <Rendering/Engine/RenderPasses/DirectionalDynamicPhysicalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalInstancedPhysicalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalTerrainShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DynamicOutlineRenderPass.h>
#include <Rendering/Engine/RenderPasses/DynamicPhysicalRenderPass.h>
#include <Rendering/Engine/RenderPasses/HighDetailTerrainRenderPass.h>
#include <Rendering/Engine/RenderPasses/InstancedPhysicalRenderPass.h>
#include <Rendering/Engine/RenderPasses/LightingRenderPass.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Rendering/Engine/RenderPasses/AboveOceanRenderPass.h>
#include <Rendering/Engine/RenderPasses/BelowOceanRenderPass.h>
#endif
#include <Rendering/Engine/RenderPasses/ParticleSystemRenderPass.h>
#include <Rendering/Engine/RenderPasses/PostProcessingRenderPass.h>
#include <Rendering/Engine/RenderPasses/SkyRenderPass.h>
#include <Rendering/Engine/RenderPasses/VegetationColorRenderPass.h>
#include <Rendering/Engine/RenderPasses/VegetationDepthRenderPass.h>
#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
#include <Rendering/Engine/RenderPasses/VolumetricFogRenderPass.h>
#endif