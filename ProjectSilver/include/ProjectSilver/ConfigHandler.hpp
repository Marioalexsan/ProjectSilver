#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Globals.hpp>
#include <fstream>

namespace Game {
	class ConfigHandler {
	private:
		static std::map<std::string, std::string> defaultConfigValues;

		static std::map<std::string, std::string> configPairs;

		static void ReadPair(std::ifstream& file);
		static void WritePair(std::ofstream& file, const std::string& key, const std::string& value);
	public:
		static bool ReadConfig(const std::string& path);
		static bool CreateDefaultConfig(const std::string& path);
		static bool SaveConfig(const std::string& path);

		static std::string RetrieveItem(const std::string& key);
		static void SetItem(const std::string& key, const std::string& value);
		static void RemoveItem(const std::string& key);



		// Specialized functions

		static std::pair<int, int> GetConfigResolution();
		static bool GetConfigFullscreen();

		static uint8_t GetMusicVolume();
		static uint8_t GetSoundVolume();
	};
}
