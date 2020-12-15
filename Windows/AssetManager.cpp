#include "AssetManager.h"

#include "GraphicsEngine.h"

namespace Game {
	AssetManager::AssetManager() {

	}

	AssetManager::~AssetManager() {
		// Only unload music if SDL is inited. If it's not... well, do nothing
		// In that case you have garbage data and a dangling non-owning pointer in your maps
		// However, since we're destructing the AssetManager, we're destructing the GameMaster too, so who cares?
		if (Mix_Init(0) != 0) {
			while (musicLibrary.size() > 0) {
				auto it = musicLibrary.begin();
				string temp = it->first;
				UnloadMusic(temp);
			}
			while (soundLibrary.size() > 0) {
				auto it = soundLibrary.begin();
				string temp = it->first;
				UnloadSound(temp);
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
			// Error
			return false;
		}

		Mix_Chunk* samples = Mix_LoadWAV(path.c_str());
		if (samples == nullptr) {
			// Error
			return false;
		}

		soundLibrary[ID] = { path, 100.0, samples };
		return true;
	}

	bool AssetManager::UnloadSound(const string& ID) {
		if (soundLibrary.find(ID) == soundLibrary.end()) {
			// Error
			return false;
		}

		Mix_FreeChunk(soundLibrary[ID].samples);
		soundLibrary.erase(ID);
		return true;
	}

	bool AssetManager::LoadMusic(const string& ID, const string& path, uint64_t duration) {
		if (musicLibrary.find(ID) != musicLibrary.end()) {
			// Error
			return false;
		}

		Mix_Music* samples = Mix_LoadMUS(path.c_str());
		if (samples == nullptr) {
			// Error
			return false;
		}

		musicLibrary[ID] = { path, 100.0, samples, duration };
		return true;
	}

	bool AssetManager::UnloadMusic(const string& ID) {
		if (musicLibrary.find(ID) == musicLibrary.end()) {
			// Error
			return false;
		}

		// Safe to free if playing
		Mix_FreeMusic(musicLibrary[ID].samples);
		musicLibrary.erase(ID);
		return true;
	}

	bool AssetManager::LoadMusicSections(const string& ID, const map<string, Section>& sectionList) {
		if (musicLibrary.find(ID) == musicLibrary.end()) {
			// Error
			return false;
		}

		// Just copy it LOL
		// Maybe setup a check for invalid sections in the future
		musicLibrary[ID].sectionList = sectionList;
		return true;
	}

	const map<string, AssetManager::MusicData>& AssetManager::GetMusicLibrary() {
		return musicLibrary;
	}

	const map<string, AssetManager::SoundData>& AssetManager::GetSoundLibrary() {
		return soundLibrary;
	}

	#pragma endregion

	AssetManager::TextureData::TextureData():
		texture(nullptr),
		path("") {}

	AssetManager::TextureData::TextureData(const string& path, SDL_Texture* texture):
		texture(texture),
		path(path) {}

	bool AssetManager::LoadTexture(const string& ID, const string& path) {
		if (textureLibrary.find(ID) != textureLibrary.end()) {
			// Error
			return false;
		}
		// Try load
		SDL_Surface* surf = IMG_Load(path.c_str());
		if (surf == nullptr) {
			// Failed to read
			return false;
		}

		//Try convert
		SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::GraphicsEngine::Renderer, surf);
		SDL_FreeSurface(surf); // Don't need this later

		if (tex == nullptr) {
			// Failed to convert
			return false;
		}

		textureLibrary[ID] = { path, tex };
		return true;
	}

	bool AssetManager::UnloadTexture(const string& ID) {
		if (textureLibrary.find(ID) == textureLibrary.end()) {
			// Error
			return false;
		}

		SDL_DestroyTexture(textureLibrary[ID].texture);
		textureLibrary.erase(ID);
		return true;
	}

	const map<string, AssetManager::TextureData>& AssetManager::GetTextureLibrary() {
		return textureLibrary;
	}
}