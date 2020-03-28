#pragma once
#include <unordered_map>

#include "alure2.h"

class FxManager final
{
private:

  typedef struct
  {
    int indices[3];
  } IPLTriangle;

  typedef struct
  {
    std::vector<alure::Vector3> vertices;
    std::vector<IPLTriangle> triangles;
  } MapData;

  std::tuple<std::string, MapData> current_map;
  std::unordered_map<std::string, MapData> map_cache;

  alure::Vector3 Normalize(const alure::Vector3& vector);
  float DotProduct(const alure::Vector3& left, const alure::Vector3& right);
  alure::Vector3 CrossProduct(const alure::Vector3& a, const alure::Vector3& b);
public:
  FxManager();

  // Checks if map is current , if not update it
  void update();

  MapData get_current_data();
};