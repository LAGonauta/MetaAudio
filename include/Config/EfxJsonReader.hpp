#pragma once

#include <fstream>
#include <unordered_map>

#include "picojson.h"
#include "efx-presets.h"

namespace MetaAudio
{
  class EfxJsonReader final
  {
  private:
    std::unordered_map<std::string, size_t> name_to_index =
    {
      {"off", 0},
      {"generic", 1},
      {"metal_small", 2},
      {"metal_medium", 3},
      {"metal_large", 4},
      {"tunnel_small", 5},
      {"tunnel_medium", 6},
      {"tunnel_large", 7},
      {"chamber_small", 8},
      {"chamber_medium", 9},
      {"chamber_large", 10},
      {"bright_small", 11},
      {"bright_medium", 12},
      {"bright_large", 13},
      {"water_1", 14},
      {"water_2", 15},
      {"water_3", 16},
      {"concrete_small", 17},
      {"concrete_medium", 18},
      {"concrete_large", 19},
      {"big_1", 20},
      {"big_2", 21},
      {"big_3", 22},
      {"cavern_small", 23},
      {"cavern_medium", 24},
      {"cavern_large", 25},
      {"weirdo_1", 26},
      {"weirdo_2", 27},
      {"weirdo_3", 28},
    };

    template<class T>
    T Get(const picojson::value& value, const std::string& effect_name)
    {
      if (value.is<T>())
      {
        return value.get<T>();
      }
      else
      {
        // console.log("Property " + propName + " for " + effect_name + " not set. Setting it to zero.")
      }

      return T();
    }

    template<>
    int Get<int>(const picojson::value& value, const std::string& effect_name)
    {
      return static_cast<int>(
        std::clamp(
          Get<int64_t>(value, effect_name),
          static_cast<int64_t>(std::numeric_limits<int>::lowest()),
          static_cast<int64_t>(std::numeric_limits<int>::max())
          )
        );
    }

    template<>
    float Get<float>(const picojson::value& value, const std::string& effect_name)
    {
      return static_cast<float>(
        std::clamp(
          Get<double>(value, effect_name),
          static_cast<double>(std::numeric_limits<float>::lowest()),
          static_cast<double>(std::numeric_limits<float>::max())
          )
        );
    }

    template<class T>
    std::vector<T> GetVector(const picojson::value& vals, const std::string& effect_name)
    {
      std::vector<T> ret;
      if (vals.is<picojson::array>())
      {
        const picojson::array& values = vals.get<picojson::array>();
        for (const picojson::value& val : values)
        {
          ret.emplace_back(Get<float>(val, effect_name));
        }
      }

      return ret;
    }

    template<class T>
    picojson::value SerializeSimpleVector(std::vector<T> vector)
    {
      picojson::array ret;

      for (const auto& value : vector)
      {
        ret.emplace_back(picojson::value(value));
      }

      return picojson::value(ret);
    }

    picojson::value SerializeEfxProps(EFXEAXREVERBPROPERTIES props, size_t index);

  public:
    std::vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> GetProperties(std::string filePath);

    std::string VectorToJson(std::vector<EFXEAXREVERBPROPERTIES> vector);
  };
}