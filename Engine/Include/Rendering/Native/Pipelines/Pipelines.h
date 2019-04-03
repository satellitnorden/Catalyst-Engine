#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/AntiAliasingPipeline.h>
#if defined(CATALYST_CONFIGURATION_DEBUG)
#include <Rendering/Native/Pipelines/DebugAxisAlignedBoundingBoxPipeline.h>
#include <Rendering/Native/Pipelines/DebugScreenBoxPipeline.h>
#endif
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#include <Rendering/Native/Pipelines/RenderOverridePipeline.h>
#endif
#include <Rendering/Native/Pipelines/ToneMappingPipeline.h>