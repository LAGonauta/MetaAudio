#pragma once

#include <fstream>
#include <optional>

#include "picojson.h"
#include "efx-presets.h"

class EfxReader final
{
private:
  std::unordered_map<alure::String, size_t> name_to_index =
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

  std::tuple<size_t, EFXEAXREVERBPROPERTIES> BuildTuple(size_t index, EFXEAXREVERBPROPERTIES prop)
  {
    return std::tuple<size_t, EFXEAXREVERBPROPERTIES>{index, prop};
  }

  template<class T>
  T Get(const picojson::value& value, const std::string& effectName)
  {
    if (value.is<T>())
    {
      return value.get<T>();
    }
    else
    {
      // console.log("Property " + propName + " for " + effectName + " not set. Setting it to zero.")
    }

    return T();
  }

  template<>
  int Get<int>(const picojson::value& value, const std::string& effectName)
  {
    return static_cast<int>(
      std::clamp(
        Get<int64_t>(value, effectName),
        static_cast<int64_t>(std::numeric_limits<int>::min()),
        static_cast<int64_t>(std::numeric_limits<int>::max())
        )
      );
  }

  template<>
  float Get<float>(const picojson::value& value, const std::string& effectName)
  {
    return static_cast<float>(
      std::clamp(
        Get<double>(value, effectName),
        static_cast<double>(std::numeric_limits<float>::min()),
        static_cast<double>(std::numeric_limits<float>::max())
        )
      );
  }

  template<class T>
  std::vector<T> GetVector(const picojson::value& vals, const std::string& effectName)
  {
    std::vector<T> ret;
    if (vals.is<picojson::array>())
    {
      const picojson::array &values = vals.get<picojson::array>();
      for (const picojson::value& val : values)
      {
        ret.push_back(Get<float>(val, effectName));
      }
    }

    return ret;
  }

public:
  alure::Vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> GetProperties(std::string filePath)
  {
    alure::Vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> ret;

    std::string fileName = "C:\\json-test\\test.json";
    std::ifstream fileStream(fileName.c_str(), std::ios::in);

    picojson::value json;
    std::string err = picojson::parse(json, fileStream);
    if (err.empty() && json.is<picojson::array>())
    {
      const picojson::array &values = json.get<picojson::array>();
      for(const picojson::value& jsonObject : values)
      {
        if (jsonObject.is<picojson::object>())
        {
          auto effectName = Get<std::string>(jsonObject.get("name"), "");
          if (!effectName.empty())
          {
            auto index = name_to_index.find(effectName);
            if (index != name_to_index.end())
            {
              auto reflectionsPan = GetVector<float>(jsonObject.get("reflectionsPan"), effectName);
              if (reflectionsPan.size() < 3)
              {
                reflectionsPan.resize(3);
              };

              auto lateReverbPan = GetVector<float>(jsonObject.get("lateReverbPan"), effectName);
              if (lateReverbPan.size() < 3)
              {
                lateReverbPan.resize(3);
              };

              EFXEAXREVERBPROPERTIES prop
              {
                Get<float>(jsonObject.get("density"), effectName),
                Get<float>(jsonObject.get("diffusion"), effectName),
                Get<float>(jsonObject.get("gain"), effectName),
                Get<float>(jsonObject.get("gainHF"), effectName),
                Get<float>(jsonObject.get("gainLF"), effectName),
                Get<float>(jsonObject.get("decayTime"), effectName),
                Get<float>(jsonObject.get("decayHFRatio"), effectName),
                Get<float>(jsonObject.get("decayLFRatio"), effectName),
                Get<float>(jsonObject.get("reflectionsGain"), effectName),
                Get<float>(jsonObject.get("reflectionsDelay"), effectName),
                { reflectionsPan[0], reflectionsPan[1], reflectionsPan[2] },
                Get<float>(jsonObject.get("lateReverbGain"), effectName),
                Get<float>(jsonObject.get("lateReverbDelay"), effectName),
                { lateReverbPan[0], lateReverbPan[1], lateReverbPan[2] },
                Get<float>(jsonObject.get("echoTime"), effectName),
                Get<float>(jsonObject.get("echoDepth"), effectName),
                Get<float>(jsonObject.get("modulationTime"), effectName),
                Get<float>(jsonObject.get("modulationDepth"), effectName),
                Get<float>(jsonObject.get("airAbsorptionGainHF"), effectName),
                Get<float>(jsonObject.get("hfReference"), effectName),
                Get<float>(jsonObject.get("lfReference"), effectName),
                Get<float>(jsonObject.get("roomRolloffFactor"), effectName),
                Get<int>(jsonObject.get("decayHFLimit"), effectName)
              };

              ret.emplace_back(BuildTuple(index->second, prop));
            }
          }
        }
      }
    }

    return ret;
  }
};