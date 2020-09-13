#pragma once
#include <unordered_map>

#include "alure2.h"
#include "dynamic_steamaudio.hpp"

namespace MetaAudio
{
  class SteamAudioMapMeshLoader final
  {
  private:
    class ProcessedMap final
    {
    private:
      std::string mapName;
      
      std::shared_ptr<IPLhandle> scene;
      std::shared_ptr<IPLhandle> static_mesh;
      std::shared_ptr<IPLhandle> environment;

    public:

      ProcessedMap() {}

      ProcessedMap(const std::string& mapName, std::shared_ptr<IPLhandle> env, std::shared_ptr<IPLhandle> scene, std::shared_ptr<IPLhandle> mesh)
        : environment(env), scene(scene), static_mesh(mesh), mapName(mapName)
      {
      }

      const std::string& Name()
      {
        return mapName;
      }

      IPLhandle Env()
      {
        return *environment;
      }
    };

    IPLSimulationSettings sa_simul_settings;
    std::shared_ptr<SteamAudio> sa = nullptr;
    std::shared_ptr<IPLhandle> sa_context = nullptr;

    std::unique_ptr<ProcessedMap> current_map;

    alure::Vector3 Normalize(const alure::Vector3& vector);
    float DotProduct(const alure::Vector3& left, const alure::Vector3& right);

    // Transmission details:
    // SteamAudio returns the transmission property of the material that was hit, not how much was transmitted
    // We should calculate ourselves how much is actually transmitted. The unit used in MetaAudio is actually
    // the attenuation `dB/m`, not how much is transmitted per meter. 
    std::array<IPLMaterial, 1> materials{ {0.10f, 0.20f, 0.30f, 0.05f, 2.0f, 4.0f, (1.0f / 0.15f)} };
  public:
    SteamAudioMapMeshLoader(std::shared_ptr<SteamAudio> sa, std::shared_ptr<IPLhandle> sa_context, IPLSimulationSettings simulSettings);

    // Checks if map is current , if not update it
    void update();

    // get current scene data as an IPLhandle
    IPLhandle CurrentEnvironment();
  };
}