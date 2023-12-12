#include <metahook.h>

#include "snd_local.h"
#include "Utilities/VectorUtils.hpp"
#include "Loaders/SteamAudioMapMeshLoader.hpp"

extern int g_iEngineType;

namespace MetaAudio
{
  constexpr const float EPSILON = 0.000001f;

  bool VectorEquals(const alure::Vector3& left, const alure::Vector3& right)
  {
    return left[0] == right[0] && left[1] == right[1] && left[2] == right[2];
  }

  bool VectorApproximatelyEquals(const alure::Vector3& left, const alure::Vector3& right)
  {
    return (left[0] - right[0]) < EPSILON && (left[1] - right[1]) < EPSILON && (left[2] - right[2]) < EPSILON;
  }

  SteamAudioMapMeshLoader::SteamAudioMapMeshLoader(SteamAudio::Context sa_context, IPLSimulationSettings simulSettings)
    : sa_simul_settings(simulSettings), sa_context(sa_context)
  {
  }

  alure::Vector3 SteamAudioMapMeshLoader::Normalize(const alure::Vector3& vector)
  {
    float length = vector.getLength();
    if (length == 0)
    {
      return alure::Vector3(0, 0, 1);
    }
    length = 1 / length;
    return alure::Vector3(vector[0] * length, vector[1] * length, vector[2] * length);
  }

  float SteamAudioMapMeshLoader::DotProduct(const alure::Vector3& left, const alure::Vector3& right)
  {
    return left[0] * right[0] + left[1] * right[1] + left[2] * right[2];
  }

  void SteamAudioMapMeshLoader::update()
  {
    // check map
    bool paused = false;
    {
      cl_entity_s* map = gEngfuncs.GetEntityByIndex(0);
      if (map == nullptr ||
          map->model == nullptr ||
          map->model->needload ||
          gEngfuncs.pfnGetLevelName() == nullptr ||
          _stricmp(gEngfuncs.pfnGetLevelName(), map->model->name) != 0)
      {
        paused = true;
      }
      else
      {
        auto mapModel = map->model;
        if (current_map != nullptr && current_map->Name() == mapModel->name)
        {
          return;
        }

        std::vector<IPLTriangle> triangles;
        std::vector<IPLVector3> vertices;
        std::vector<IPLint32> materialIndices;

        for (int i = 0; i < mapModel->nummodelsurfaces; ++i)
        {
          glpoly_t* firstPoly = nullptr;
          if (g_iEngineType == ENGINE_GOLDSRC_HL25)
          {
              msurface_hl25_t* surfaces = reinterpret_cast<msurface_hl25_t*>(mapModel->surfaces);
              msurface_hl25_t* surface = &surfaces[mapModel->firstmodelsurface + i];
              firstPoly = surface->polys;
          }
          else
          {
              msurface_t* surface = &mapModel->surfaces[mapModel->firstmodelsurface + i];
              firstPoly = surface->polys;
          }
          std::vector<alure::Vector3> surfaceVerts;
          glpoly_t* poly = firstPoly;
          while (poly)
          {
            if (poly->numverts <= 0)
            {
              continue;
            }

            for (int j = 0; j < poly->numverts; ++j)
            {
              surfaceVerts.emplace_back(MetaAudio::AL_UnpackVector(poly->verts[j]));
            }

            poly = poly->next;

            // escape rings
            if (poly == firstPoly)
            {
              break;
            }
          }

          // triangulation

          // Get rid of duplicate vertices
          surfaceVerts.erase(std::unique(surfaceVerts.begin(), surfaceVerts.end(), VectorEquals), surfaceVerts.end());

          // Skip invalid face
          if (surfaceVerts.size() < 3)
          {
            continue;
          }

          // Triangulate
          alure::Vector3 origin{ 0,0,0 };
          alure::Vector<IPLVector3> newVerts;
          { // remove colinear
            for (size_t i = 0; i < surfaceVerts.size(); ++i)
            {
              auto vertexBefore = origin + surfaceVerts[(i > 0) ? (i - 1) : (surfaceVerts.size() - 1)];
              auto vertex = origin + surfaceVerts[i];
              auto vertexAfter = origin + surfaceVerts[(i < (surfaceVerts.size() - 1)) ? (i + 1) : 0];

              auto v1 = Normalize(vertexBefore - vertex);
              auto v2 = Normalize(vertexAfter - vertex);

              float vertDot = DotProduct(v1, v2);
              if (std::fabs(vertDot + 1.f) < EPSILON)
              {
                // colinear, drop it
              }
              else
              {
                newVerts.emplace_back<IPLVector3>({ vertex[0], vertex[1], vertex[2] });
              }
            }
          }

          // Skip invalid face, it is just a line
          if (newVerts.size() < 3)
          {
            continue;
          }

          { // generate indices
            int indexoffset = vertices.size();

            for (size_t i = 0; i < newVerts.size() - 2; ++i)
            {
              auto& triangle = triangles.emplace_back();

              triangle.indices[0] = indexoffset + i + 2;
              triangle.indices[1] = indexoffset + i + 1;
              triangle.indices[2] = indexoffset;

              // in the future, each triangle will have its own texture set
              // which wil be used to select the proper material
              materialIndices.push_back(0);
            }

            // Add vertices to final array
            vertices.insert(vertices.end(), newVerts.begin(), newVerts.end());
          }
        }

        IPLSceneSettings settings{};
        settings.type = IPLSceneType::IPL_SCENETYPE_DEFAULT;
        auto sceneResult = this->sa_context.CreateScene(settings);
		if (std::holds_alternative<IPLerror>(sceneResult) && std::get<1>(sceneResult) != IPLerror::IPL_STATUS_SUCCESS) {
            // TODO: log error and return instead of throw?
            throw std::runtime_error("Error creating scene: " + std::to_string(std::get<1>(sceneResult)));
            //return;
        }

        auto& scene = std::get<0>(sceneResult);
        IPLStaticMeshSettings staticMeshSettings{};
        staticMeshSettings.numVertices = vertices.size();
        staticMeshSettings.vertices = vertices.data();
        staticMeshSettings.numTriangles = triangles.size();
        staticMeshSettings.triangles = triangles.data();
        staticMeshSettings.numMaterials = materials.size();
        staticMeshSettings.materials = materials.data();
        staticMeshSettings.materialIndices = materialIndices.data();

        auto staticMeshResult = scene.StaticMeshCreate(staticMeshSettings);
		if (std::holds_alternative<IPLerror>(staticMeshResult) && std::get<1>(staticMeshResult) != IPLerror::IPL_STATUS_SUCCESS) {
            throw std::runtime_error("Error creating static mesh: " + std::to_string(std::get<1>(staticMeshResult)));
        }
        auto& staticMesh = std::get<0>(staticMeshResult);
        scene.StaticMeshAdd(staticMesh);
        scene.Commit();

        auto simulatorResult = sa_context.CreateSimulator(sa_simul_settings);
		if (std::holds_alternative<IPLerror>(simulatorResult) && std::get<1>(simulatorResult) != IPLerror::IPL_STATUS_SUCCESS) {
            throw std::runtime_error("Error creating simulator: " + std::to_string(std::get<1>(simulatorResult)));
        }
        auto& simulator = std::get<0>(simulatorResult);
        simulator.SetScene(scene);
        simulator.Commit();

        current_map = std::make_unique<ProcessedMap>(mapModel->name, scene, staticMesh, simulator);
      }
    }
  }

  SteamAudio::Simulator SteamAudioMapMeshLoader::CurrentSimulator()
  {
    return current_map->Simulator();
  }
}