#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/AntiAliasingRenderPass.h>
#if defined(CATALYST_CONFIGURATION_DEBUG)
#include <Rendering/Engine/RenderPasses/DebugAxisAlignedBoundingBoxRenderPass.h>
#include <Rendering/Engine/RenderPasses/DebugScreenBoxRenderPass.h>
#endif
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#include <Rendering/Engine/RenderPasses/RenderOverrideRenderPass.h>
#endif
#include <Rendering/Engine/RenderPasses/ToneMappingRenderPass.h>