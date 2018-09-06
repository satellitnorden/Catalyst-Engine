#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/DirectionalInstancedPhysicalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalStaticPhysicalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DirectionalTerrainShadowRenderPass.h>
#include <Rendering/Engine/RenderPasses/DynamicOutlineRenderPass.h>
#include <Rendering/Engine/RenderPasses/DynamicPhysicalRenderPass.h>
#include <Rendering/Engine/RenderPasses/InstancedPhysicalRenderPass.h>
#include <Rendering/Engine/RenderPasses/LightingRenderPass.h>
#if !defined(CATALYST_DISABLE_OCEAN)
#include <Rendering/Engine/RenderPasses/OceanRenderPass.h>
#endif
#include <Rendering/Engine/RenderPasses/ParticleSystemRenderPass.h>
#include <Rendering/Engine/RenderPasses/PostProcessingRenderPass.h>
#include <Rendering/Engine/RenderPasses/SkyRenderPass.h>
#include <Rendering/Engine/RenderPasses/StaticPhysicalRenderPass.h>
#include <Rendering/Engine/RenderPasses/TerrainRenderPass.h>
#include <Rendering/Engine/RenderPasses/VegetationRenderPass.h>