#pragma once
#include <unordered_map>

#include "alure2.h"
#include "phonon.h"

namespace MetaAudio
{
  class SteamAudioMapMeshLoader final
  {
  private:
    typedef struct
    {
      std::vector<alure::Vector3> vertices;
      std::vector<IPLTriangle> triangles;
    } MapData;

    class CacheItem final
    {
    private:
      IPLhandle environment;
      IPLhandle scene;
      IPLhandle static_mesh;

    public:
      CacheItem(IPLhandle env, IPLhandle scene, IPLhandle mesh) : environment(env), scene(scene), static_mesh(mesh)
      {
      }

      ~CacheItem()
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
      CacheItem(const CacheItem& other) = delete;
      CacheItem& CacheItem::operator=(const CacheItem& other) = delete;

      // allow move
      CacheItem(CacheItem&& other) noexcept
      {
        std::swap(environment, other.environment);
        std::swap(scene, other.scene);
        std::swap(static_mesh, other.static_mesh);
      }
      CacheItem& operator=(CacheItem&& other) noexcept
      {
        std::swap(environment, other.environment);
        std::swap(scene, other.scene);
        std::swap(static_mesh, other.static_mesh);
        return *this;
      }

      IPLhandle Env()
      {
        return environment;
      }
    };

    IPLSimulationSettings sa_simul_settings;
    IPLhandle sa_context;

    std::tuple<std::string, IPLhandle> current_env;
    std::unordered_map<std::string, std::shared_ptr<CacheItem>> map_cache;

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
    IPLhandle get_current_environment();

    void PurgeCache();
  };
}