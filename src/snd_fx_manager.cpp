#include <metahook.h>

#include "Utilities/VectorUtils.hpp"
#include "snd_local.h"
#include "snd_fx_manager.hpp"

constexpr const float EPSILON = 0.000001f;

bool VectorEquals(const alure::Vector3& left, const alure::Vector3& right)
{
  return left[0] == right[0] && left[1] == right[1] && left[2] == right[2];
}

bool VectorApproximatelyEquals(const alure::Vector3& left, const alure::Vector3& right)
{
  return (left[0] - right[0]) < EPSILON && (left[1] - right[1]) < EPSILON && (left[2] - right[2]) < EPSILON;
}

FxManager::FxManager()
{
  current_map = std::make_tuple("", MapData{});
}

alure::Vector3 FxManager::Normalize(const alure::Vector3& vector)
{
  float length = vector.getLength();
  if (length == 0)
  {
    return alure::Vector3(0, 0, 1);
  }
  length = 1 / length;
  return alure::Vector3(vector[0] * length, vector[1] * length, vector[2] * length);
}

float FxManager::DotProduct(const alure::Vector3& left, const alure::Vector3& right)
{
  return left[0] * right[0] + left[1] * right[1] + left[2] * right[2];
}

alure::Vector3 FxManager::CrossProduct(const alure::Vector3& a, const alure::Vector3& b)
{
  return alure::Vector3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}

void FxManager::update()
{
  // check map
  bool paused = false;
  {
    cl_entity_s* map = gEngfuncs.GetEntityByIndex(0);
    if (map == nullptr
      || map->model == nullptr
      || map->model->needload
      || gEngfuncs.pfnGetLevelName() == nullptr
      || _stricmp(gEngfuncs.pfnGetLevelName(), map->model->name) != 0)
    {
      paused = true;
    }
    else
    {
      auto mapModel = map->model;
      if (std::get<0>(current_map) == mapModel->name)
      {
        return;
      }
      else
      {
        auto search = map_cache.find(mapModel->name);
        if (search != map_cache.end())
        {
          current_map = std::make_tuple(search->first, search->second);
          return;
        }
      }

      std::vector<IPLTriangle> triangles;
      std::vector<alure::Vector3> triangulatedVerts;

      for (int i = 0; i < mapModel->nummodelsurfaces; ++i)
      {
        auto surface = mapModel->surfaces[mapModel->firstmodelsurface + i];
        glpoly_t* poly = surface.polys;
        std::vector<alure::Vector3> surfaceVerts;
        while (poly)
        {
          if (poly->numverts <= 0)
            continue;

          for (int j = 0; j < poly->numverts; j++)
          {
            surfaceVerts.emplace_back(MetaAudio::AL_UnpackVector(poly->verts[j]));
          }

          poly = poly->next;

          // escape rings
          if (poly == surface.polys)
            break;
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
        alure::Vector<alure::Vector3> newVerts;
        { // remove colinear
          for (size_t i = 0; i < surfaceVerts.size(); ++i)
          {
            alure::Vector3 vertexBefore = origin + surfaceVerts[(i > 0) ? (i - 1) : (surfaceVerts.size() - 1)];
            alure::Vector3 vertex = origin + surfaceVerts[i];
            alure::Vector3 vertexAfter = origin + surfaceVerts[(i < (surfaceVerts.size() - 1)) ? (i + 1) : 0];

            alure::Vector3 v1 = Normalize(vertexBefore - vertex);
            alure::Vector3 v2 = Normalize(vertexAfter - vertex);

            float vertDot = DotProduct(v1, v2);
            if (std::fabs(vertDot + 1.f) < EPSILON)
            {
              // colinear, drop it
            }
            else
            {
              newVerts.emplace_back(vertex);
            }
          }
        }

        // Skip invalid face, it is just a line
        if (newVerts.size() < 3)
        {
          continue;
        }

        { // generate indices
          int indexoffset = triangulatedVerts.size();
          auto actualNormal = MetaAudio::AL_CopyVector(surface.plane->normal);

          for (size_t i = 0; i < newVerts.size() - 2; ++i)
          {
            auto& triangle = triangles.emplace_back();

            triangle.indices[0] = indexoffset + i + 2;
            triangle.indices[1] = indexoffset + i + 1;
            triangle.indices[2] = indexoffset;

            // Faster way to check if the ordering is correct?
            auto normal = Normalize(CrossProduct(newVerts[triangle.indices[0] - indexoffset] - newVerts[triangle.indices[1] - indexoffset],
                                                 newVerts[triangle.indices[1] - indexoffset] - newVerts[triangle.indices[2] - indexoffset]));
            if (!VectorApproximatelyEquals(normal, actualNormal))
            {
              std::swap(triangle.indices[1], triangle.indices[2]);
            }
          }

          // Add vertices to final array
          triangulatedVerts.insert(triangulatedVerts.end(), newVerts.begin(), newVerts.end());
        }
      }

      map_cache[mapModel->name] = MapData{ triangulatedVerts, triangles};
      current_map = std::make_tuple(mapModel->name, map_cache[mapModel->name]);
    }
  }
}

FxManager::MapData FxManager::get_current_data()
{
  return std::get<1>(current_map);
}