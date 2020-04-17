#pragma once
#include <unordered_map>

#include "alure2.h"
#include "phonon.h"

namespace MetaAudio
{
  class SteamAudioMapMeshLoader final
  {
  private:
    class ProcessedMap final
    {
    private:
      std::string mapName;
      IPLhandle environment;
      IPLhandle scene;
      IPLhandle static_mesh;

    public:
      ProcessedMap(const std::string& mapName, IPLhandle env, IPLhandle scene, IPLhandle mesh)
        : environment(env), scene(scene), static_mesh(mesh), mapName(mapName)
      {
      }

      ~ProcessedMap()
      {
        if (environment != nullptr)
        {
          iplDestroyEnvironment(&environment);
        }

        if (scene != nullptr)
        {
          iplDestroyScene(&scene);
        }

        if (static_mesh != nullptr)
        {
          iplDestroyStaticMesh(&static_mesh);
        }
      }

      // delete copy
      ProcessedMap(const ProcessedMap& other) = delete;
      ProcessedMap& ProcessedMap::operator=(const ProcessedMap& other) = delete;

      // allow move
      ProcessedMap(ProcessedMap&& other) noexcept
      {
        std::swap(environment, other.environment);
        std::swap(scene, other.scene);
        std::swap(static_mesh, other.static_mesh);
      }
      ProcessedMap& operator=(ProcessedMap&& other) noexcept
      {
        std::swap(environment, other.environment);
        std::swap(scene, other.scene);
        std::swap(static_mesh, other.static_mesh);
        return *this;
      }

      const std::string& Name()
      {
        return mapName;
      }

      IPLhandle Env()
      {
        return environment;
      }
    };

    IPLSimulationSettings sa_simul_settings;
    IPLhandle sa_context;

    std::unique_ptr<ProcessedMap> current_map;

    alure::Vector3 Normalize(const alure::Vector3& vector);
    float DotProduct(const alure::Vector3& left, const alure::Vector3& right);

    // Transmission details:
    // SteamAudio returns the transmission property of the material that was hit, not how much was transmitted
    // We should calculate ourselves how much is actually transmitted. The unit used in MetaAudio is actually
    // the attenuation `dB/m`, not how much is transmitted per meter. 
    std::array<IPLMaterial, 1> materials{ {0.10f, 0.20f, 0.30f, 0.05f, 2.0f, 4.0f, (1.0f / 0.15f)} };
  public:
    SteamAudioMapMeshLoader(IPLhandle sa_context, IPLSimulationSettings simulSettings);

    // Checks if map is current , if not update it
    void update();

    // get current scene data as an IPLhandle
    IPLhandle CurrentEnvironment();
  };
}