#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderPasses/AntiAliasingRenderPass.h>
#if defined(CATALYST_CONFIGURATION_DEBUG)
#include <Rendering/Native/RenderPasses/DebugAxisAlignedBoundingBoxRenderPass.h>
#include <Rendering/Native/RenderPasses/DebugScreenBoxRenderPass.h>
#endif
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#include <Rendering/Native/RenderPasses/RenderOverrideRenderPass.h>
#endif
#include <Rendering/Native/RenderPasses/ToneMappingRenderPass.h>