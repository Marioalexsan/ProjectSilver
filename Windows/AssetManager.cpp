#include "PCHeader.h"

#include "AssetManager.h"
#include "GraphicsEngine.h"

#pragma region Helper Functions

void DiscardWord(std::ifstream& file, char delim = ' ') {
	file >> std::ws;
	file.ignore(std::numeric_limits<std::streamsize>::max(), delim);
}

void ReadWordPair(std::ifstream& file, pair<string, string>& dst, char delim = '\"') {
	file >> std::ws;
	getline(file, dst.first, '=');
	file.ignore();
	getline(file, dst.second, delim);
}

void ReadField(std::ifstream& file, pair<string, int>& dst, char delim = ' ') {
	file >> std::ws;
	getline(file, dst.first, '=');
	string temp;
	getline(file, temp, delim);
	dst.second = std::stoi(temp);
}

void ReadPadding(std::ifstream& file, int* padArray, char delim = ' ') {
	file >> std::ws;
	file.ignore(std::numeric_limits<std::streamsize>::max(), '=');
	string temp;
	for (int i = 0; i < 3; i++) {
		getline(file, temp, ',');
		padArray[i] = std::stoi(temp);
	}
	getline(file, temp, delim);
	padArray[3] = std::stoi(temp);
}

void ReadSpacing(std::ifstream& file, int* spaceArray, char delim = ' ') {
	file >> std::ws;
	file.ignore(std::numeric_limits<std::streamsize>::max(), '=');
	string temp;
	getline(file, temp, ',');
	spaceArray[0] = std::stoi(temp);
	getline(file, temp, delim);
	spaceArray[1] = std::stoi(temp);
}

#pragma endregion

namespace Game {
	AssetManager::AssetManager() {

	}

	AssetManager::~AssetManager() {
		// Only unload music if SDL is inited. If it's not... well, do nothing
		// In that case you have garbage data and a dangling non-owning pointer in your maps
		// However, since we're destructing the AssetManager, we're destructing the GameMaster too, so who cares?
		if (Mix_Init(0) != 0) {
			while (musicLibrary.size() > 0) {
				UnloadMusic(musicLibrary.begin()->first);
			}
			while (soundLibrary.size() > 0) {
				UnloadSound(soundLibrary.begin()->first);
			}
			while (textureLibrary.size() > 0) {
				UnloadTexture(textureLibrary.begin()->first);
			}
			while (fontLibrary.size() > 0) {
				UnloadSpriteFont(fontLibrary.begin()->first);
			}
		}
	}

	#pragma region Audio Assets

	AssetManager::MusicData::MusicData() :
		sectionList(),
		path(),
		baseVolume(0.0),
		samples(nullptr),
		duration(0) {}

	AssetManager::MusicData::MusicData(const string& path, double baseVolume, Mix_Music* samples, uint64_t duration) :
		sectionList(),
		path(path),
		baseVolume(baseVolume),
		samples(samples),
		duration(duration) {}

	AssetManager::SoundData::SoundData() :
		path(),
		baseVolume(0.0),
		samples(nullptr) {}

	AssetManager::SoundData::SoundData(const string& path, double baseVolume, Mix_Chunk* samples) :
		path(path),
		baseVolume(baseVolume),
		samples(samples) {}

	bool AssetManager::LoadSound(const string& ID, const string& path) {
		if (soundLibrary.find(ID) != soundLibrary.end()) {
			return false;
		}

		Mix_Chunk* samples = Mix_LoadWAV(path.c_str());
		if (samples == nullptr) {
			Game::LogHandler::Log("Couldn't load sound " + path + ", ID " + ID, Game::LogHandler::MessageType::Warn);
			return false;
		}

		soundLibrary[ID] = { path, 100.0, samples };
		return true;
	}

	bool AssetManager::LoadMusic(const string& ID, const string& path, uint64_t duration) {
		if (musicLibrary.find(ID) != musicLibrary.end()) {
			return false;
		}

		Mix_Music* samples = Mix_LoadMUS(path.c_str());
		if (samples == nullptr) {
			Game::LogHandler::Log("Couldn't load music " + path + ", ID " + ID, Game::LogHandler::MessageType::Warn);
			return false;
		}

		musicLibrary[ID] = { path, 100.0, samples, duration };
		return true;
	}

	bool AssetManager::UnloadSound(const string& ID) {
		if (soundLibrary.find(ID) == soundLibrary.end()) {
			return false;
		}

		Mix_FreeChunk(soundLibrary[ID].samples);
		soundLibrary.erase(ID);
		return true;
	}

	bool AssetManager::UnloadMusic(const string& ID) {
		if (musicLibrary.find(ID) == musicLibrary.end()) {
			return false;
		}

		// Safe to free if playing
		Mix_FreeMusic(musicLibrary[ID].samples);
		musicLibrary.erase(ID);
		return true;
	}

	bool AssetManager::LoadMusicSections(const string& ID, const map<string, Section>& sectionList) {
		if (musicLibrary.find(ID) == musicLibrary.end()) {
			Game::LogHandler::Log("Tried to load sections for unknown music " + ID, Game::LogHandler::MessageType::Warn);
			return false;
		}

		// Just copy it LOL
		// Maybe setup a check for invalid sections in the future
		musicLibrary[ID].sectionList = sectionList;
		return true;
	}

	#pragma endregion

	#pragma region Graphical Data

	AssetManager::TextureData::TextureData():
		texture(nullptr),
		path("") {}

	AssetManager::TextureData::TextureData(const string& path, SDL_Texture* texture):
		texture(texture),
		path(path) {}

	bool AssetManager::LoadTexture(const string& ID, const string& path) {
		if (textureLibrary.find(ID) != textureLibrary.end()) {
			return false;
		}
		// Try load
		SDL_Surface* surf = IMG_Load(path.c_str());
		if (surf == nullptr) {
			Game::LogHandler::Log("Couldn't load image " + path + ", ID " + ID, Game::LogHandler::MessageType::Warn);
			return false;
		}

		//Try convert
		SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::GraphicsEngine::Renderer, surf);
		SDL_FreeSurface(surf); // Don't need this later

		if (tex == nullptr) {
			Game::LogHandler::Log("Failed to create texture for image " + path + ", ID " + ID, Game::LogHandler::MessageType::Warn);
			return false;
		}

		textureLibrary[ID] = { path, tex };
		return true;
	}

	bool AssetManager::LoadSpriteFont(const string& ID, const string& texPath, const string& dataPath) {
		// Functia asta se putea de scris mai bine....
		
		if (fontLibrary.find(ID) != fontLibrary.end()) {
			return false;
		}

		SpriteFontData data;
		
		std::ifstream fontData(dataPath);
		if (!fontData.good()) {
			return false;
		}
		map<string, int> arbitraries;

		int section = 0;
		try {
			using std::getline;
			pair<string, string> strpair;
			pair<string, int> field;

			// Info section
			DiscardWord(fontData);
			ReadWordPair(fontData, strpair);
			data.face = strpair.second;

			for (int i = 0; i < 3; i++) {
				ReadField(fontData, field);
				data.arbitraryValues[field.first] = field.second;
			}

			ReadWordPair(fontData, strpair);
			data.charset = strpair.second;

			for (int i = 0; i < 4; i++) {
				ReadField(fontData, field);
				data.arbitraryValues[field.first] = field.second;
			}

			ReadPadding(fontData, data.padding);
			ReadSpacing(fontData, data.spacing);

			ReadField(fontData, field, '\n');
			data.arbitraryValues[field.first] = field.second;

			// Common section
			DiscardWord(fontData);

			for (int i = 0; i < 9; i++) {
				ReadField(fontData, field);
				data.arbitraryValues[field.first] = field.second;
			}
			ReadField(fontData, field, '\n');
			data.arbitraryValues[field.first] = field.second;

			// Page section

			DiscardWord(fontData);
			ReadField(fontData, field); // Discard
			ReadWordPair(fontData, strpair);
			data.file = strpair.second;

			// Chars header
			DiscardWord(fontData);
			ReadField(fontData, field, '\n');
			int count = field.second;

			for (int i = 0; i < count; i++) {
				// Read chars
				DiscardWord(fontData);
				ReadField(fontData, field);
				int index = field.second;

				ReadField(fontData, field);
				data.charLibrary[index].x = field.second;
				ReadField(fontData, field);
				data.charLibrary[index].y = field.second;
				ReadField(fontData, field);
				data.charLibrary[index].width = field.second;
				ReadField(fontData, field);
				data.charLibrary[index].height = field.second;
				ReadField(fontData, field);
				data.charLibrary[index].xoffset = field.second;
				ReadField(fontData, field);
				data.charLibrary[index].yoffset = field.second;
				ReadField(fontData, field);
				data.charLibrary[index].xadvance = field.second;
				ReadField(fontData, field); // Discard
				ReadField(fontData, field, '\n');
				data.charLibrary[index].channel = field.second;
			}

		}
		catch (...) {
			fontData.close();
			Game::LogHandler::Log("An exception happened while reading font data, ID " + ID, Game::LogHandler::MessageType::Warn);
			Game::LogHandler::Log("Data Path: " + dataPath, Game::LogHandler::MessageType::Warn);
			return false;
		}

		fontData.close();

		// Try load
		SDL_Surface* surf = IMG_Load(texPath.c_str());
		if (surf == nullptr) {
			Game::LogHandler::Log("Couldn't load font texture, ID  " + ID, Game::LogHandler::MessageType::Warn);
			Game::LogHandler::Log("Texture Path: " + texPath, Game::LogHandler::MessageType::Warn);
			return false;
		}

		//Try convert
		SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::GraphicsEngine::Renderer, surf);
		SDL_FreeSurface(surf); // Don't need this later

		if (tex == nullptr) {
			Game::LogHandler::Log("Failed to create texture for font, ID " + ID, Game::LogHandler::MessageType::Warn);
			Game::LogHandler::Log("Texture Path: " + texPath, Game::LogHandler::MessageType::Warn);
			return false;
		}

		TextureData texData = { texPath, tex };

		fontLibrary[ID] = std::make_pair(data, texData);
		return true;
	}

	bool AssetManager::UnloadTexture(const string& ID) {
		if (textureLibrary.find(ID) == textureLibrary.end()) {
			return false;
		}

		SDL_DestroyTexture(textureLibrary[ID].texture);
		textureLibrary.erase(ID);
		return true;
	}

	bool AssetManager::UnloadSpriteFont(const string& ID) {
		if (fontLibrary.find(ID) == fontLibrary.end()) {
			return false;
		}

		SDL_DestroyTexture(fontLibrary[ID].second.texture);
		fontLibrary.erase(ID);
		return true;
	}

	#pragma endregion
}