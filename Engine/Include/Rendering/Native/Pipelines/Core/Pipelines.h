#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AntiAliasingPipeline.h>
#if defined(CATALYST_CONFIGURATION_DEBUG)
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugAxisAlignedBoundingBoxPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugScreenBoxPipeline.h>
#endif
#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#include <Rendering/Native/Pipelines/GraphicsPipelines/RenderOverridePipeline.h>
#endif
#include <Rendering/Native/Pipelines/GraphicsPipelines/ToneMappingPipeline.h>