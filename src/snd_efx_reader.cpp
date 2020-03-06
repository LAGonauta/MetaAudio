#include "snd_efx_reader.hpp"

std::vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> EfxReader::GetProperties(std::string filePath)
{
  std::vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> ret;

  std::ifstream fileStream(filePath.c_str(), std::ios::in);

  picojson::value json;
  std::string err = picojson::parse(json, fileStream);
  if (err.empty() && json.is<picojson::array>())
  {
    const picojson::array &values = json.get<picojson::array>();
    for (const picojson::value& jsonObject : values)
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

            ret.emplace_back(std::make_tuple(index->second, prop));
          }
        }
      }
    }
  }

  return ret;
}

std::string EfxReader::VectorToJson(std::vector<EFXEAXREVERBPROPERTIES> vector)
{
  picojson::array jsonArray;
  for (size_t i = 0; i < vector.size(); ++i)
  {
    jsonArray.emplace_back(SerializeEfxProps(vector[i], i));
  }

  return picojson::value(jsonArray).serialize();
}

picojson::value EfxReader::SerializeEfxProps(EFXEAXREVERBPROPERTIES props, size_t index)
{
  picojson::object ret;

  auto name = std::find_if(name_to_index.begin(), name_to_index.end(), [&index](const auto& vt) { return vt.second == index; });
  if (name != name_to_index.end())
  {
    ret["name"] = picojson::value(name->first);
  }

  ret["density"] = picojson::value(props.flDensity);
  ret["diffusion"] = picojson::value(props.flDiffusion);
  ret["gain"] = picojson::value(props.flGain);
  ret["gainHF"] = picojson::value(props.flGainHF);
  ret["gainLF"] = picojson::value(props.flGainLF);
  ret["decayTime"] = picojson::value(props.flDecayTime);
  ret["decayHFRatio"] = picojson::value(props.flDecayHFRatio);
  ret["decayLFRatio"] = picojson::value(props.flDecayLFRatio);
  ret["reflectionsGain"] = picojson::value(props.flReflectionsGain);
  ret["reflectionsDelay"] = picojson::value(props.flReflectionsDelay);
  ret["reflectionsPan"] = SerializeSimpleVector<float>(std::vector<float>{ props.flReflectionsPan[0], props.flReflectionsPan[1], props.flReflectionsPan[2] });
  ret["lateReverbGain"] = picojson::value(props.flLateReverbGain);
  ret["lateReverbDelay"] = picojson::value(props.flLateReverbDelay);
  ret["lateReverbPan"] = SerializeSimpleVector<float>(std::vector<float>{ props.flLateReverbPan[0], props.flLateReverbPan[1], props.flLateReverbPan[2] });
  ret["echoTime"] = picojson::value(props.flEchoTime);
  ret["echoDepth"] = picojson::value(props.flEchoDepth);
  ret["modulationTime"] = picojson::value(props.flModulationTime);
  ret["modulationDepth"] = picojson::value(props.flModulationDepth);
  ret["airAbsorptionGainHF"] = picojson::value(props.flAirAbsorptionGainHF);
  ret["hfReference"] = picojson::value(props.flHFReference);
  ret["lfReference"] = picojson::value(props.flLFReference);
  ret["roomRolloffFactor"] = picojson::value(props.flRoomRolloffFactor);
  ret["decayHFLimit"] = picojson::value(static_cast<int64_t>(props.iDecayHFLimit));

  return picojson::value(ret);
}