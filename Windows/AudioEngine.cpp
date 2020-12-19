#include "PCHeader.h"

#include "AudioEngine.h"

namespace Game {
	AudioEngine* AudioEngine::currentEngine = nullptr;

	AudioEngine::AudioEngine() :
		currentTicks(SDL_GetTicks()),
		channels{ 0 },
		currentID(1),
		soundCount(0),
		musicVolume(75),
		soundVolume(75),
		manager(nullptr)
	{
		if (currentEngine == nullptr) {
			currentEngine = this;
		}
		Mix_AllocateChannels(maxChannels);
	}

	AudioEngine::~AudioEngine() {
		if (Mix_Init(0) != 0) {
			Mix_AllocateChannels(0);
		}
		if (currentEngine == this) {
			currentEngine = nullptr;
		}
	}

	void AudioEngine::SetContentManager(Game::AssetManager* manager) {
		this->manager = manager;
	}


	const map<string, AssetManager::MusicData>& AudioEngine::GetMusicLib() {
		return manager->GetMusicLibrary();
	}
	const map<string, AssetManager::SoundData>& AudioEngine::GetSoundLib() {
		return manager->GetSoundLibrary();
	}

	const AssetManager::MusicData& AudioEngine::SearchMusicLib(const string& ID) {
		return manager->GetMusicLibrary().at(ID);
	}

	const AssetManager::SoundData& AudioEngine::SearchSoundLib(const string& ID){
		return manager->GetSoundLibrary().at(ID);
	}

	uint64_t AudioEngine::PlaySound(const string& ID, const Vector2 pos) {
		if (soundCount >= maxChannels) {
			// Auto fail
			return 0;
		}
		if (GetSoundLib().find(ID) == GetSoundLib().end()) {
			// Error
			return 0;
		}
		int channel = Mix_PlayChannel(-1, SearchSoundLib(ID).samples, 0);

		if (channel == -1) {
			// Error
			return 0;
		}

		// Set Volume
		Mix_Volume(channel, int(double(soundVolume) * 100.0 / 128.0));

		auto nextID = NextID();

		// I suppose there's no other ID of that type
		sounds[nextID] = { ID, pos, channel };

		channels[channel] = nextID;
		soundCount++;
		return nextID;
	}

	bool AudioEngine::StopSound(uint64_t ID) {
		if (sounds.find(ID) == sounds.end()) {
			// Error
			return false;
		}

		int channel = sounds[ID].channel;

		Mix_HaltChannel(channel);
		channels[channel] = 0;
		sounds.erase(ID);
		soundCount--;
		return true;
	}

	bool AudioEngine::StopSound(const string& dataID) {
		bool foundAny = false;
		for (int channel = 0; channel < maxChannels; channel++) {
			auto ID = channels[channel];
			if (ID != 0 && sounds.find(ID) != sounds.end() && sounds[ID].dataID == dataID) {
				Mix_HaltChannel(channel);
				channels[channel] = 0;
				sounds.erase(ID);
				soundCount--;
				foundAny = true;
			}
		}
		return foundAny;
	}

	uint64_t AudioEngine::NextID() {
		uint64_t val = currentID++;
		currentID = currentID == 0 ? 1 : currentID;
		return val;
	}

	void AudioEngine::Update() {
		// Update info
		uint32_t time = SDL_GetTicks();
		uint32_t delta = time - currentTicks;
		currentTicks = time;

		// Update music pos
		if (Mix_PlayingMusic() != 0) {
			music.timePos += delta;
			// Check for music loop requirement
			if (!(music.loopEnd == 0 && music.loopStart == 0)) {
				if (music.timePos > music.loopEnd) {
					Mix_SetMusicPosition(double(music.loopStart) / 1000.0);
					music.timePos = music.loopStart;
				}
			}
		}

		// Check for stopped sounds
		for (int channel = 0; channel < maxChannels; channel++) {
			if (channels[channel] != 0) {
				// Sound theoretically playing
				if (Mix_Playing(channel) == 0) {
					// Sound is actually done
					sounds.erase(channels[channel]);
					soundCount--;
					channels[channel] = 0;
				}
			}
		}

		// Process music actions
		bool doNextAction = true;
		while (doNextAction) {
			doNextAction = false;
			if (actionQueue.size() == 0) {
				//Nothin to do LOL
				break;
			}
			auto action = actionQueue.front();

			switch (action.type) {
			case MusicAction::Type::None:
				// Nothing
				actionQueue.pop();
				break;
			case MusicAction::Type::Start:
				if (Mix_PlayingMusic() == 0) {
					// GOGOGOGOGOGOGO, play and discard action
					if (Mix_FadeInMusicPos(SearchMusicLib(action.param).samples, -1, defaultFadeTime, double(action.extraParams[0]) / 1000.0) == -1) {
						// Error
						break;
					}
					Mix_VolumeMusic(int(double(musicVolume) * 100.0 / 128.0));
					music = { action.param, action.extraParams[0], 0, 0 };

					//Also do next action in sequence, *if* possible
					actionQueue.pop();
					doNextAction = true;
				}
				break;
			case MusicAction::Type::Stop:
				if (Mix_PlayingMusic() != 0) {
					// Try to stop (even if already doing so)
					Mix_FadeOutMusic(defaultFadeTime);
				}
				else {
					// GOGOGOGO
					music.dataID = "";
					actionQueue.pop();
					doNextAction = true;
				}
				break;
			case MusicAction::Type::Seek: {
				if (manager->GetMusicLibrary().find(music.dataID) == manager->GetMusicLibrary().end()) {
					// Broken
					actionQueue.pop();
					break;
				}
				auto sectionList = SearchMusicLib(music.dataID).sectionList;
				if (sectionList.find(action.param) == sectionList.end()) {
					// Broken
					actionQueue.pop();
					break;
				}

				switch (action.subType) {
				case MusicAction::SubType::None:
					if (Mix_PlayingMusic() != 0) {
						// Use fast fade
						Mix_FadeOutMusic(int(defaultFadeTime * 0.4));
						actionQueue.front().subType = MusicAction::SubType::FadeOutSection;
					}
					break;
				case MusicAction::SubType::FadeOutSection:
					if (Mix_PlayingMusic() == 0) {
						music.timePos = SearchMusicLib(music.dataID).sectionList.at(action.param).start;
						Mix_FadeInMusicPos(SearchMusicLib(music.dataID).samples, -1, int(defaultFadeTime * 0.4), double(music.timePos) / 1000.0);
						actionQueue.front().subType = MusicAction::SubType::FadeInSection;
					}
					break;
				case MusicAction::SubType::FadeInSection:
					if (Mix_FadingMusic() == 0) {
						// Done with the switch
						actionQueue.pop();
						doNextAction = true;
					}
					break;
				}
			}
				break;
			case MusicAction::Type::SetLoopSection:
				if (Mix_PlayingMusic() != 0) {
					music.loopStart = SearchMusicLib(music.dataID).sectionList.at(action.param).start;
					music.loopEnd = SearchMusicLib(music.dataID).sectionList.at(action.param).end;
					actionQueue.pop();
					doNextAction = true;
				}
			default:
				// Lmao
				break;
			}
		}
	}

	int AudioEngine::GetSoundCount() {
		return soundCount;
		// Maybe use Mix_Playing(-1) instead
	}

	

	void AudioEngine::AddAction(MusicAction::Type type, const string& param, const vector<uint64_t>& extraParams, bool useFade) {
		MusicAction lol = { type, MusicAction::SubType::None, param, extraParams, useFade };
		actionQueue.push(lol);
	}

	bool AudioEngine::PlayMusic(const string& ID, uint64_t timePos) {
		// Unlike sounds, there's only one music playing at a time
		// Partly due to SDL Mixer implementation
		if (GetMusicLib().find(ID) == GetMusicLib().end()) {
			// Error
			return false;
		}

		if (Mix_PlayingMusic()) {
			AddAction(MusicAction::Type::Stop);
		}
		AddAction(MusicAction::Type::Start, ID, { timePos });

		// Probably bad, but inits everything NOW, so it's needed
		Update();

		return true;
	}

	bool AudioEngine::StopMusic() {
		if (Mix_PlayingMusic() == 0) {
			//Lmao
			return false;
		}
		AddAction(MusicAction::Type::Stop);
		return true;
	}

	void AudioEngine::HaltEngine() {
		queue<AudioEngine::MusicAction>().swap(actionQueue); // Empties queue
		Mix_HaltMusic();
		for (auto& it : sounds) {
			Mix_HaltChannel(it.second.channel);
		}
		music.dataID = "";
		sounds.clear();
	}

	bool AudioEngine::SetLoopSection(const string& section) {
		if (GetMusicLib().find(music.dataID) == GetMusicLib().end()) {
			// Not likely, buuuuuttt....
			return false;
		}
		auto sectionList = SearchMusicLib(music.dataID).sectionList;
		if (sectionList.find(section) == sectionList.end()) {
			// Not found LOL
			return false;
		}
		AddAction(MusicAction::Type::SetLoopSection, section);
		return true;
	}

	bool AudioEngine::SeekToSection(const string& section) {
		AddAction(MusicAction::Type::Seek, section);
		return true;
	}

	uint64_t AudioEngine::GetMusicPosition() {
		return music.timePos;
	}

	bool AudioEngine::IsMusicPlaying() {
		return Mix_PlayingMusic();
	}

	bool AudioEngine::IsEngineIdle() {
		Update();
		return actionQueue.size() == 0 && !Mix_PlayingMusic() && Mix_Playing(-1) == 0;
	}
}