#include "PCHeader.h"

#include "Globals.h" // Also includes the required AssetManager
#include "AudioEngine.h"
#include "ConfigHandler.h"

namespace Game {
	AudioEngine::AudioEngine() :
		currentTicks(SDL_GetTicks()),
		channels{ 0 },
		currentID(1),
		soundCount(0),
		musicVolume(100),
		soundVolume(80),
		userMusicVolume(100),
		userSoundVolume(100)
	{
		// Music and audio volumes for the user are set later by the Game, during mega init
		Mix_AllocateChannels(maxChannels);
	}

	AudioEngine::~AudioEngine() {
		if (Mix_Init(0) != 0) {
			Mix_AllocateChannels(0); // Destructor might be called after Mix_Quit() was called
		}
	}

	void AudioEngine::AddAction(MusicAction::Type type, const string& param, const vector<uint64_t>& extraParams, bool useFade) {
		MusicAction lol = { type, MusicAction::SubType::None, param, extraParams, useFade };
		actionQueue.push(lol);
	}

	const map<string, AssetManager::MusicData>& AudioEngine::GetMusicLib() {
		return Globals::Assets().GetMusicLibrary();
	}
	const map<string, AssetManager::SoundData>& AudioEngine::GetSoundLib() {
		return Globals::Assets().GetSoundLibrary();
	}

	const AssetManager::MusicData& AudioEngine::SearchMusicLib(const string& ID) {
		return Globals::Assets().GetMusicLibrary().at(ID);
	}

	const AssetManager::SoundData& AudioEngine::SearchSoundLib(const string& ID){
		return Globals::Assets().GetSoundLibrary().at(ID);
	}

	uint64_t AudioEngine::PlaySound(const string& ID, const Vector2 pos) {
		if (soundCount >= maxChannels || GetSoundLib().find(ID) == GetSoundLib().end()) {
			return 0;
		}

		// It's better to search through our own records first. At least, I think this fixes a bug where sounds won't play from time to time
		int firstFreeChannel = -1;
		for (int index = 0; index < maxChannels; index++) {
			if (channels[index] == 0) {
				firstFreeChannel = index;
				break;
			}
		}

		int channel = Mix_PlayChannel(firstFreeChannel, SearchSoundLib(ID).samples, 0);
		if (channel == -1) {
			return 0;
		}

		Mix_Volume(channel, int(double(soundVolume) * userSoundVolume / 100.0)); // Max is 128, but (as a convention), we can just use 0-100. That way, the sound won't be stupidly loud.

		auto nextID = NextID();
		sounds[nextID] = { ID, pos, channel };
		channels[channel] = nextID;
		soundCount++;
		return nextID;
	}

	bool AudioEngine::StopSound(uint64_t ID) {
		if (sounds.find(ID) == sounds.end()) {
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
		uint32_t time = SDL_GetTicks();
		uint32_t delta = time - currentTicks;
		currentTicks = time;

		// Update music pos
		if (Mix_PlayingMusic() != 0) {
			music.timePos += delta;
			// Check for music loop requirement
			if (!(music.loopEnd == 0 && music.loopStart == 0)) {
				if (music.timePos > music.loopEnd) {
					music.timePos = music.loopStart + music.timePos - music.loopEnd;
					Mix_SetMusicPosition(double(music.timePos) / 1000.0);
					
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
					if (Mix_FadeInMusicPos(SearchMusicLib(action.param).samples, -1, musicSwitchFadeTime, double(action.extraParams[0]) / 1000.0) == -1) {
						// Error
						break;
					}
					Mix_VolumeMusic(int(double(musicVolume) * userMusicVolume / 100.0)); // Max is 128, but we'll use the range 0-100
					music = { action.param, action.extraParams[0], 0, 0 };

					//Also do next action in sequence, *if* possible
					actionQueue.pop();
					doNextAction = true;
				}
				break;
			case MusicAction::Type::Stop:
				if (Mix_PlayingMusic() != 0) {
					// Try to stop (even if already doing so)
					Mix_FadeOutMusic(musicSwitchFadeTime);
				}
				else {
					// GOGOGOGO
					music.dataID = "";
					actionQueue.pop();
					doNextAction = true;
				}
				break;
			case MusicAction::Type::Seek: {
				if (GetMusicLib().find(music.dataID) == GetMusicLib().end()) {
					// Broken
					actionQueue.pop();
					break;
				}
				auto& sectionList = SearchMusicLib(music.dataID).sectionList;
				if (sectionList.find(action.param) == sectionList.end()) {
					// Broken
					actionQueue.pop();
					break;
				}

				switch (action.subType) {
				case MusicAction::SubType::None:
					if (Mix_PlayingMusic() != 0) {
						// Use fast fade
						Mix_FadeOutMusic(int(sectionSwitchFadeTime * 0.4));
						actionQueue.front().subType = MusicAction::SubType::FadeOutSection;
					}
					break;
				case MusicAction::SubType::FadeOutSection:
					if (Mix_PlayingMusic() == 0) {
						music.timePos = SearchMusicLib(music.dataID).sectionList.at(action.param).start;
						Mix_FadeInMusicPos(SearchMusicLib(music.dataID).samples, -1, int(sectionSwitchFadeTime * 0.4), double(music.timePos) / 1000.0);
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
				} break;
			case MusicAction::Type::ChangeVolume:
				if (Mix_PlayingMusic() != 0) {
					Mix_VolumeMusic(int(musicVolume / 100.0 * 128.0 * double(userMusicVolume) / 100.0));
				} 
				actionQueue.pop();
				doNextAction = true;
				break;
			default:
				// Lmao
				break;
			}
		}
	}



	bool AudioEngine::PlayMusic(const string& ID, uint64_t timePos) {
		// Unlike sounds, there's only one music playing at a time
		// Partly due to SDL Mixer implementation
		if (GetMusicLib().find(ID) == GetMusicLib().end()) {
			Game::LogHandler::Log("Couldn't play music " + ID, Game::LogHandler::MessageType::Warn);
			return false;
		}

		if (Mix_PlayingMusic()) {
			AddAction(MusicAction::Type::Stop);
		}
		AddAction(MusicAction::Type::Start, ID, { timePos });

		// Run an update to init actions
		Update();

		return true;
	}

	bool AudioEngine::StopMusic() {
		if (Mix_PlayingMusic() == 0) {
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

	void AudioEngine::SetUserMusicVolume(double volume)
	{
		userMusicVolume = (int)Utility::ClampValue(volume, 0.0, 100.0);
		AddAction(MusicAction::Type::ChangeVolume, "");
	}
	void AudioEngine::SetUserSoundVolume(double volume) {
		userSoundVolume = (int)Utility::ClampValue(volume, 0.0, 100.0);
		Mix_Volume(-1, int(soundVolume / 100.0 * 128.0 * double(userSoundVolume) / 100.0));
	}

	void AudioEngine::SetMusicVolume(double volume) {
		musicVolume = (int)Utility::ClampValue(volume, 0.0, 100.0);
		AddAction(MusicAction::Type::ChangeVolume, "");
	}
	void AudioEngine::SetSoundVolume(double volume) {
		soundVolume = (int)Utility::ClampValue(volume, 0.0, 100.0);
		Mix_Volume(-1, int(soundVolume / 100.0 * 128.0 * double(userSoundVolume) / 100.0));
	}

	bool AudioEngine::SetLoopSection(const string& section) {
		if (GetMusicLib().find(music.dataID) == GetMusicLib().end()) {
			return false;
		}
		auto& sectionList = SearchMusicLib(music.dataID).sectionList;
		if (sectionList.find(section) == sectionList.end()) {
			Game::LogHandler::Log("Couldn't set loop section " + section + ". Current music playing: \"" + music.dataID + "\"", Game::LogHandler::MessageType::Warn);
			return false;
		}
		AddAction(MusicAction::Type::SetLoopSection, section);
		return true;
	}

	bool AudioEngine::SeekToSection(const string& section) {
		AddAction(MusicAction::Type::Seek, section);
		return true;
	}

	bool AudioEngine::IsEngineIdle() {
		Update(); // Improves accuracy
		return actionQueue.size() == 0 && !Mix_PlayingMusic() && Mix_Playing(-1) == 0;
	}
}