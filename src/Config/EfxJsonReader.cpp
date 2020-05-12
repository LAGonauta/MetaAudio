#include "Config/EfxJsonReader.hpp"

namespace MetaAudio
{
  std::vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> EfxJsonReader::GetProperties(std::string filePath)
  {
    std::vector<std::tuple<size_t, EFXEAXREVERBPROPERTIES>> ret;

    std::ifstream file_stream(filePath, std::ios::in);

    picojson::value json;
    std::string err = picojson::parse(json, file_stream);
    if (err.empty() && json.is<picojson::array>())
    {
      const picojson::array& values = json.get<picojson::array>();
      for (const picojson::value& jsonObject : values)
      {
        if (jsonObject.is<picojson::object>())
        {
          auto effect_name = Get<std::string>(jsonObject.get("name"), "");
          if (!effect_name.empty())
          {
            auto index = name_to_index.find(effect_name);
            if (index != name_to_index.end())
            {
              auto reflections_pan = GetVector<float>(jsonObject.get("reflectionsPan"), effect_name);
              if (reflections_pan.size() < 3)
              {
                reflections_pan.resize(3);
              };

              auto late_reverb_pan = GetVector<float>(jsonObject.get("lateReverbPan"), effect_name);
              if (late_reverb_pan.size() < 3)
              {
                late_reverb_pan.resize(3);
              };

              EFXEAXREVERBPROPERTIES prop
              {
                Get<float>(jsonObject.get("density"), effect_name),
                Get<float>(jsonObject.get("diffusion"), effect_name),
                Get<float>(jsonObject.get("gain"), effect_name),
                Get<float>(jsonObject.get("gainHF"), effect_name),
                Get<float>(jsonObject.get("gainLF"), effect_name),
                Get<float>(jsonObject.get("decayTime"), effect_name),
                Get<float>(jsonObject.get("decayHFRatio"), effect_name),
                Get<float>(jsonObject.get("decayLFRatio"), effect_name),
                Get<float>(jsonObject.get("reflectionsGain"), effect_name),
                Get<float>(jsonObject.get("reflectionsDelay"), effect_name),
                { reflections_pan[0], reflections_pan[1], reflections_pan[2] },
                Get<float>(jsonObject.get("lateReverbGain"), effect_name),
                Get<float>(jsonObject.get("lateReverbDelay"), effect_name),
                { late_reverb_pan[0], late_reverb_pan[1], late_reverb_pan[2] },
                Get<float>(jsonObject.get("echoTime"), effect_name),
                Get<float>(jsonObject.get("echoDepth"), effect_name),
                Get<float>(jsonObject.get("modulationTime"), effect_name),
                Get<float>(jsonObject.get("modulationDepth"), effect_name),
                Get<float>(jsonObject.get("airAbsorptionGainHF"), effect_name),
                Get<float>(jsonObject.get("hfReference"), effect_name),
                Get<float>(jsonObject.get("lfReference"), effect_name),
                Get<float>(jsonObject.get("roomRolloffFactor"), effect_name),
                Get<int>(jsonObject.get("decayHFLimit"), effect_name)
              };

              ret.emplace_back(std::make_tuple(index->second, prop));
            }
          }
        }
      }
    }

    return ret;
  }

  std::string EfxJsonReader::VectorToJson(std::vector<EFXEAXREVERBPROPERTIES> vector)
  {
    picojson::array json_array;
    for (size_t i = 0; i < vector.size(); ++i)
    {
      json_array.emplace_back(SerializeEfxProps(vector[i], i));
    }

    return picojson::value(json_array).serialize();
  }

  picojson::value EfxJsonReader::SerializeEfxProps(EFXEAXREVERBPROPERTIES props, size_t index)
  {
    picojson::object ret;

    auto name = std::find_if(name_to_index.begin(), name_to_index.end(), [=](const auto& vt) { return vt.second == index; });
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
}