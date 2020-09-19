#pragma once
#include <unordered_map>

#include "alure2.h"
#include "DynamicSteamAudio.hpp"

namespace MetaAudio
{
  class SteamAudioMapMeshLoader final
  {
  private:
    class ProcessedMap final
    {
    private:
      std::string mapName;
      
      alure::SharedPtr<IPLhandle> scene;
      alure::SharedPtr<IPLhandle> static_mesh;
      alure::SharedPtr<IPLhandle> environment;

    public:

      ProcessedMap() {}

      ProcessedMap(const std::string& mapName, alure::SharedPtr<IPLhandle> env, alure::SharedPtr<IPLhandle> scene, alure::SharedPtr<IPLhandle> mesh)
        : environment(env), scene(scene), static_mesh(mesh), mapName(mapName)
      {
      }

      const std::string& Name()
      {
        return mapName;
      }

      alure::SharedPtr<IPLhandle> Env()
      {
        return environment;
      }
    };

    IPLSimulationSettings sa_simul_settings;
    alure::SharedPtr<SteamAudio> sa = nullptr;
    alure::SharedPtr<IPLhandle> sa_context = nullptr;

    alure::SharedPtr<ProcessedMap> current_map;

    alure::Vector3 Normalize(const alure::Vector3& vector);
    float DotProduct(const alure::Vector3& left, const alure::Vector3& right);

    // Transmission details:
    // SteamAudio returns the transmission property of the material that was hit, not how much was transmitted
    // We should calculate ourselves how much is actually transmitted. The unit used in MetaAudio is actually
    // the attenuation `dB/m`, not how much is transmitted per meter. 
    std::array<IPLMaterial, 1> materials{ {0.10f, 0.20f, 0.30f, 0.05f, 2.0f, 4.0f, (1.0f / 0.15f)} };
  public:
    SteamAudioMapMeshLoader(alure::SharedPtr<SteamAudio> sa, alure::SharedPtr<IPLhandle> sa_context, IPLSimulationSettings simulSettings);

    // Checks if map is current , if not update it
    void update();

    // get current scene data as an IPLhandle
    alure::SharedPtr<IPLhandle> CurrentEnvironment();
  };
}