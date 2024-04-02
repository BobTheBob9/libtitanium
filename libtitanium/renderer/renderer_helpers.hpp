#pragma once

#include "renderer.hpp"

namespace renderer::helpers
{
    GPUFunctionArgs DEVTEMP_CreatePlane( Device *const pRenderDevice );
    void DEVTEMP_CreateUniformArgs( Device *const pRenderDevice, GPUFunction *const pgpuFunc, GPUFunctionArgs *const pArgs, size_t uniformSize, const void *const uniformValue );
    void DEVTEMP_UpdateUniformArgs( Device *const pRenderDevice, GPUFunctionArgs *const pArgs, size_t uniformSize, const void *const uniformValue );
}
