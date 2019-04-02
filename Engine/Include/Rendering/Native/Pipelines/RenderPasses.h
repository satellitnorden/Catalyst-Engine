#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/AntiAliasingRenderPass.h>
#if defined(CATALYST_CONFIGURATION_DEBUG)
#include <Rendering/Native/Pipelines/DebugAxisAlignedBoundingBoxRenderPass.h>
#include <Rendering/Native/Pipelines/DebugScreenBoxRenderPass.h>
#endif
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#include <Rendering/Native/Pipelines/RenderOverrideRenderPass.h>
#endif
#include <Rendering/Native/Pipelines/ToneMappingRenderPass.h>