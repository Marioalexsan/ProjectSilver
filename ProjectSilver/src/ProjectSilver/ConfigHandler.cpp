module;

#include <ProjectSilver/PCHeader.hpp>

module ProjectSilver.ConfigHandler;
import ProjectSilver.MiscUtility;

namespace Game
{
    std::map<std::string, std::string> ConfigHandler::defaultConfigValues =
        {{"resolution", "1280.720"},
         {"windowmode", "w"},
         {"musicvolume", "90"},
         {"soundvolume", "100"}};

    std::map<std::string, std::string> ConfigHandler::configPairs;

    void ConfigHandler::ReadPair(std::ifstream& file)
    {
        if (file)
        {
            file >> std::ws;
            std::string key, value;
            file >> key;
            if (file.eof())
            {
                return;
            }
            file >> std::ws;
            std::getline(file, value);
            configPairs[key] = value;
        }
    }

    void ConfigHandler::WritePair(std::ofstream&     file,
                                  const std::string& key,
                                  const std::string& value)
    {
        if (file)
        {
            int         start        = (int)key.find_first_not_of(" \n\r\t");
            std::string trimmedKey   = key.substr(start,
                                                key.find_last_not_of(" \n\r\t") - start + 1);
            start                    = (int)value.find_first_not_of(" \n\r\t");
            std::string trimmedValue = value.substr(start,
                                                    value.find_last_not_of(" \n\r\t") -
                                                        start + 1);

            file << key << " " << value << std::endl;
        }
    }

    bool ConfigHandler::ReadConfig(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
        {
            return false;
        }

        configPairs.clear();
        while (!file.eof())
        {
            ReadPair(file);
        }

        for (auto& elem : defaultConfigValues)
        {
            if (configPairs.find(elem.first) == configPairs.end())
            {
                configPairs[elem.first] = elem.second;
            }
        }

        file.close();

        return true;
    }

    bool ConfigHandler::CreateDefaultConfig(const std::string& path)
    {
        std::ofstream file(path);
        if (!file)
        {
            return false;
        }
        configPairs.clear();

        for (auto& elem : defaultConfigValues)
        {
            WritePair(file, elem.first, elem.second);
        }

        return true;
    }

    bool ConfigHandler::SaveConfig(const std::string& path)
    {
        std::ofstream file(path);
        if (!file)
        {
            return false;
        }

        for (auto& elem : configPairs)
        {
            WritePair(file, elem.first, elem.second);
        }

        file.close();

        return true;
    }

    std::string ConfigHandler::RetrieveItem(const std::string& key)
    {
        std::string item = "";
        if (configPairs.find(key) != configPairs.end())
        {
            item = configPairs[key];
        }
        return item;
    }

    void ConfigHandler::SetItem(const std::string& key, const std::string& value)
    {
        configPairs[key] = value;
    }

    void ConfigHandler::RemoveItem(const std::string& key)
    {
        configPairs.erase(key);
    }

    std::pair<int, int> ConfigHandler::GetConfigResolution()
    {
        std::string cfgResolution = ConfigHandler::RetrieveItem("resolution");
        int         width         = 800;
        int         height        = 600;

        std::string cfgResolutionWidth = cfgResolution.substr(0, cfgResolution.find('.'));
        if (cfgResolutionWidth != "")
        {
            width = std::stoi(cfgResolutionWidth);
        }

        std::string cfgResolutionHeight = cfgResolution.substr(cfgResolution.find('.') + 1);
        if (cfgResolutionHeight != "")
        {
            height = std::stoi(cfgResolutionHeight);
        }

        if (width <= 800)
        {
            width = 800;
        }
        if (height <= 600)
        {
            height = 600;
        }
        return {width, height};
    }

    bool ConfigHandler::GetConfigFullscreen()
    {
        return configPairs.find("windowmode") != configPairs.end() &&
               configPairs.at("windowmode") == "f";
    }

    uint8_t ConfigHandler::GetMusicVolume()
    {
        int returnVal = 100;
        if (configPairs.find("musicvolume") != configPairs.end())
        {
            std::string value = configPairs.at("musicvolume");
            if (value != "")
            {
                returnVal = std::stoi(value);
            }
        }
        returnVal = Utility::ClampValue(returnVal, 0, 100);
        return (uint8_t)returnVal;
    }

    uint8_t ConfigHandler::GetSoundVolume()
    {
        int returnVal = 100;
        if (configPairs.find("soundvolume") != configPairs.end())
        {
            std::string value = configPairs.at("soundvolume");
            if (value != "")
            {
                returnVal = std::stoi(value);
            }
        }
        returnVal = Utility::ClampValue(returnVal, 0, 100);
        return (uint8_t)returnVal;
    }
} // namespace Game