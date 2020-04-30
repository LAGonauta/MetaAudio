#pragma once

#include "phonon.h"

namespace MetaAudio
{
  struct SteamAudio
  {
    void (*iplDestroyEnvironment)(IPLhandle* env);
    void (*iplDestroyScene)(IPLhandle* scene);
    void (*iplDestroyStaticMesh)(IPLhandle* staticMesh);
    IPLDirectSoundPath (*iplGetDirectSoundPath)(
      IPLhandle environment,
      IPLVector3 listenerPosition,
      IPLVector3 listenerAhead,
      IPLVector3 listenerUp,
      IPLSource source,
      IPLfloat32 sourceRadius,
      IPLint32 numSamples,
      IPLDirectOcclusionMode occlusionMode,
      IPLDirectOcclusionMethod occlusionMethod
      );
    IPLerror (*iplCreateScene)(IPLhandle context, IPLhandle computeDevice,
      IPLSceneType sceneType, IPLint32 numMaterials,
      IPLMaterial* materials, IPLClosestHitCallback closestHitCallback,
      IPLAnyHitCallback anyHitCallback,
      IPLBatchedClosestHitCallback batchedClosestHitCallback,
      IPLBatchedAnyHitCallback batchedAnyHitCallback, IPLvoid* userData,
      IPLhandle* scene);
    IPLerror (*iplCreateStaticMesh)(IPLhandle scene, IPLint32 numVertices, IPLint32 numTriangles,
      IPLVector3* vertices, IPLTriangle* triangles, IPLint32* materialIndices,
      IPLhandle* staticMesh);
    IPLerror (*iplCreateEnvironment)(IPLhandle context, IPLhandle computeDevice,
      IPLSimulationSettings simulationSettings, IPLhandle scene, IPLhandle probeManager, IPLhandle* environment);
    IPLerror (*iplCreateContext)(IPLLogFunction logCallback,
      IPLAllocateFunction allocateCallback,
      IPLFreeFunction freeCallback,
      IPLhandle* context);
    IPLvoid (*iplDestroyContext)(IPLhandle* context);
    IPLvoid (*iplCleanup)();
  };

#define SetSteamAudioFunctionPointer(target, library, __function_name__) target.__function_name__ = reinterpret_cast<decltype(MetaAudio::SteamAudio::__function_name__)>(GetProcAddress(library, #__function_name__));
}

extern MetaAudio::SteamAudio gSteamAudio;