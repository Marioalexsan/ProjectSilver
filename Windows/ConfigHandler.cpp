#include "PCHeader.h"
#include "ConfigHandler.h"

namespace Game {
	map<string, string> ConfigHandler::defaultConfigValues = {
		{"resolution", "1280.720"},
		{"windowmode", "w"},
		{"musicvolume", "90"},
		{"soundvolume", "100"}
	};

	map<string, string> ConfigHandler::configPairs;

	void ConfigHandler::ReadPair(std::ifstream& file) {
		if (file) {
			file >> std::ws;
			string key, value;
			file >> key;
			if (file.eof()) {
				return;
			}
			file >> std::ws;
			std::getline(file, value);
			configPairs[key] = value;
		}
	}

	void ConfigHandler::WritePair(std::ofstream& file, const string& key, const string& value) {
		if (file) {
			int start = key.find_first_not_of(" \n\r\t");
			string trimmedKey = key.substr(start, key.find_last_not_of(" \n\r\t") - start + 1);
			start = value.find_first_not_of(" \n\r\t");
			string trimmedValue = value.substr(start, value.find_last_not_of(" \n\r\t") - start + 1);

			file << key << " " << value << endl;
		}
	}

	bool ConfigHandler::ReadConfig(const string& path) {
		std::ifstream file(path);
		if (!file) {
			return false;
		}

		configPairs.clear();
		while (!file.eof()) {
			ReadPair(file);
		}

		for (auto& elem : defaultConfigValues) {
			if (configPairs.find(elem.first) == configPairs.end()) {
				configPairs[elem.first] = elem.second;
			}
		}

		file.close();

		return true;
	}

	bool ConfigHandler::CreateDefaultConfig(const string& path) {
		std::ofstream file(path);
		if (!file) {
			return false;
		}
		configPairs.clear();

		for (auto& elem : defaultConfigValues) {
			WritePair(file, elem.first, elem.second);
		}

		return true;
	}

	bool ConfigHandler::SaveConfig(const string& path) {
		std::ofstream file(path);
		if (!file) {
			return false;
		}
		
		for (auto& elem : configPairs) {
			WritePair(file, elem.first, elem.second);
		}

		file.close();

		return true;
	}

	string ConfigHandler::RetrieveItem(const string& key) {
		string item = "";
		if (configPairs.find(key) != configPairs.end()) {
			item = configPairs[key];
		}
		return item;
	}

	void ConfigHandler::SetItem(const string& key, const string& value) {
		configPairs[key] = value;
	}

	void ConfigHandler::RemoveItem(const string& key) {
		configPairs.erase(key);
	}

	pair<int, int> ConfigHandler::GetConfigResolution() {
		auto cfgResolution = ConfigHandler::RetrieveItem("resolution");
		int width = 1280;
		int height = 720;

		auto cfgResolutionWidth = cfgResolution.substr(0, cfgResolution.find('.'));
		if (cfgResolutionWidth != "") {
			width = std::stoi(cfgResolutionWidth);
		}

		auto cfgResolutionHeight = cfgResolution.substr(cfgResolution.find('.') + 1);
		if (cfgResolutionHeight != "") {
			height = std::stoi(cfgResolutionHeight);
		}
		
		if (width <= 1280) {
			width = 1280;
		}
		if (height <= 720) {
			height = 720;
		}
		return { width, height };
	}

	bool ConfigHandler::GetConfigFullscreen() {
		return configPairs.find("windowmode") != configPairs.end() && configPairs.at("windowmode") == "f";
	}

	uint8_t ConfigHandler::GetMusicVolume() {
		int returnVal = 100;
		if (configPairs.find("musicvolume") != configPairs.end())
		{
			string value = configPairs.at("musicvolume");
			if (value != "") {
				returnVal = std::stoi(value);
			}
		}
		returnVal = Utility::ClampValue(returnVal, 0, 100);
		return (uint8_t)returnVal;
	}

	uint8_t ConfigHandler::GetSoundVolume() {
		int returnVal = 100;
		if (configPairs.find("soundvolume") != configPairs.end())
		{
			string value = configPairs.at("soundvolume");
			if (value != "") {
				returnVal = std::stoi(value);
			}
		}
		returnVal = Utility::ClampValue(returnVal, 0, 100);
		return (uint8_t)returnVal;
	}
}