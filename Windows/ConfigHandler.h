#ifndef CONFIG_HEADER
#define CONFIG_HEADER

#include "PCHeader.h"
#include "Globals.h"
#include <fstream>

namespace Game {
	class ConfigHandler {
	private:
		static map<string, string> defaultConfigValues;

		static map<string, string> configPairs;

		static void ReadPair(std::ifstream& file);
		static void WritePair(std::ofstream& file, const string& key, const string& value);
	public:
		static bool ReadConfig(const string& path);
		static bool CreateDefaultConfig(const string& path);
		static bool SaveConfig(const string& path);

		static string RetrieveItem(const string& key);
		static void SetItem(const string& key, const string& value);
		static void RemoveItem(const string& key);



		// Specialized functions

		static pair<int, int> GetConfigResolution();
		static bool GetConfigFullscreen();

		static uint8_t GetMusicVolume();
		static uint8_t GetSoundVolume();
	};
}

#endif