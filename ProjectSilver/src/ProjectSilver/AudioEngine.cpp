#include <ProjectSilver/AudioEngine.hpp>
#include <ProjectSilver/ConfigHandler.hpp>
#include <ProjectSilver/Globals.hpp> // Also includes the required AssetManager
#include <ProjectSilver/LogHandler.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    AudioEngine::AudioEngine() :
    currentID(1),
    musicVolume(100),
    soundVolume(80),
    userMusicVolume(100),
    userSoundVolume(100)
    {
    }

    AudioEngine::~AudioEngine()
    {
    }

    void AudioEngine::AddAction(MusicAction::Type            type,
                                const std::string&           param,
                                const std::vector<uint64_t>& extraParams,
                                bool                         useFade)
    {
        MusicAction lol = {type, MusicAction::SubType::None, param, extraParams, useFade};
        actionQueue.push(lol);
    }

    const std::map<std::string, AssetManager::MusicData>& AudioEngine::GetMusicLib()
    {
        return Globals::Assets().GetMusicLibrary();
    }

    const std::map<std::string, AssetManager::SoundData>& AudioEngine::GetSoundLib()
    {
        return Globals::Assets().GetSoundLibrary();
    }

    const AssetManager::MusicData& AudioEngine::SearchMusicLib(const std::string& ID)
    {
        return Globals::Assets().GetMusicLibrary().at(ID);
    }

    const AssetManager::SoundData& AudioEngine::SearchSoundLib(const std::string& ID)
    {
        return Globals::Assets().GetSoundLibrary().at(ID);
    }

    uint64_t AudioEngine::PlaySound(const std::string& ID, const Vector2 pos)
    {
        if (sounds.size() >= 128 || GetSoundLib().find(ID) == GetSoundLib().end())
        {
            return 0;
        }

        auto sound = std::make_unique<sf::Sound>(*SearchSoundLib(ID).samples);
        sound->setVolume(float(soundVolume) * userSoundVolume / 100.f);
        sound->play();

        auto nextID    = NextID();
        sounds[nextID] = std::move(sound);
        return nextID;
    }

    bool AudioEngine::StopSound(uint64_t ID)
    {
        if (sounds.find(ID) == sounds.end())
        {
            return false;
        }

        sounds[ID]->stop();
        sounds.erase(ID);
        return true;
    }

    bool AudioEngine::StopSound(const std::string& dataID)
    {
        bool                       foundAny = false;
        std::vector<std::uint64_t> soundsToStop;
        for (auto& [key, sound] : soundDataIDs)
        {
            if (sound == dataID)
                soundsToStop.push_back(key);
        }

        for (auto sound : soundsToStop)
        {
            soundDataIDs.erase(sound);
            sounds.erase(sound);
        }

        return soundsToStop.size() > 0;
    }

    uint64_t AudioEngine::NextID()
    {
        uint64_t val = currentID++;
        currentID    = currentID == 0 ? 1 : currentID;
        return val;
    }

    void AudioEngine::Update()
    {
        sf::Time delta = stopwatch.restart();

        // Check for stopped sounds
        std::vector<std::uint64_t> soundsToStop;
        for (auto& [key, sound] : sounds)
        {
            if (sound->getStatus() == sf::Sound::Stopped)
            {
                soundsToStop.push_back(key);
            }
        }

        for (auto sound : soundsToStop)
        {
            soundDataIDs.erase(sound);
            sounds[sound]->stop();
            sounds.erase(sound);
        }

        if (fadeToSilence)
        {
            fadeVolumeMultiplier = std::clamp(fadeVolumeMultiplier - delta / fadeDuration,
                                              0.f,
                                              1.f);
        }
        else
        {
            fadeVolumeMultiplier = std::clamp(fadeVolumeMultiplier + delta / fadeDuration,
                                              0.f,
                                              1.f);
        }

        // Update music volume
        if (music)
        {
            music->setVolume(
                fadeVolumeMultiplier * int(double(musicVolume) * userMusicVolume / 100.0));
        }

        // Process music actions
        bool doNextAction = true;
        while (doNextAction)
        {
            doNextAction = false;
            if (actionQueue.size() == 0)
            {
                //Nothin to do LOL
                break;
            }
            auto& action = actionQueue.front();

            switch (action.type)
            {
                case MusicAction::Type::None:
                    // Nothing
                    actionQueue.pop();
                    break;
                case MusicAction::Type::Start:
                    if (!music || music->getStatus() == sf::Music::Stopped)
                    {
                        // GOGOGOGOGOGOGO, play and discard action
                        music = SearchMusicLib(action.param).music.get();
                        music->setPlayingOffset(
                            sf::seconds(float(action.extraParams[0]) / 1000.f));
                        music->setVolume(fadeVolumeMultiplier *
                                         int(float(musicVolume) * userMusicVolume / 100.f));
                        music->setLoop(true);
                        music->play();
                        musicID       = action.param;
                        fadeToSilence = false;
                        fadeDuration  = sf::milliseconds(musicSwitchFadeTime);

                        //Also do next action in sequence, *if* possible
                        actionQueue.pop();
                        doNextAction = true;
                    }
                    break;
                case MusicAction::Type::Stop:
                    if (music && music->getStatus() == sf::Music::Playing && !fadeToSilence)
                    {
                        fadeToSilence = true;
                        fadeDuration  = sf::milliseconds(musicSwitchFadeTime);
                    }
                    else if (fadeVolumeMultiplier == 0.f)
                    {
                        // GOGOGOGO
                        musicID = "";
                        if (music)
                            music->stop();
                        actionQueue.pop();
                        doNextAction = true;
                    }
                    break;
                case MusicAction::Type::Seek:
                {
                    if (GetMusicLib().find(musicID) == GetMusicLib().end())
                    {
                        // Broken
                        actionQueue.pop();
                        break;
                    }
                    auto& sectionList = SearchMusicLib(musicID).sectionList;
                    if (sectionList.find(action.param) == sectionList.end())
                    {
                        // Broken
                        actionQueue.pop();
                        break;
                    }

                    switch (action.subType)
                    {
                        case MusicAction::SubType::None:
                            if (music && music->getStatus() == sf::Music::Playing)
                            {
                                // Use fast fade
                                fadeToSilence = true;
                                fadeDuration = sf::milliseconds(musicSwitchFadeTime * 4 / 10);
                                actionQueue.front().subType = MusicAction::SubType::FadeOutSection;
                            }
                            break;
                        case MusicAction::SubType::FadeOutSection:
                            if (!music || music->getStatus() == sf::Music::Stopped)
                            {
                                auto& data = SearchMusicLib(musicID);

                                music->setPlayingOffset(sf::seconds(
                                    data.sectionList.at(action.param).start / 1000.f));
                                actionQueue.front().subType = MusicAction::SubType::FadeInSection;
                            }
                            break;
                        case MusicAction::SubType::FadeInSection:
                            if (fadeVolumeMultiplier >= 1.0f)
                            {
                                // Done with the switch
                                actionQueue.pop();
                                doNextAction = true;
                            }
                            break;
                    }
                }
                break;
                case MusicAction::Type::SetLoopSection:
                    if (music)
                    {
                        auto& data = SearchMusicLib(musicID);
                        music->setLoopPoints(
                            {sf::seconds(data.sectionList.at(action.param).start / 1000.0f),
                             sf::seconds((data.sectionList.at(action.param).end -
                                          data.sectionList.at(action.param).start) /
                                         1000.0f)});
                        actionQueue.pop();
                        doNextAction = true;
                    }
                    break;
                case MusicAction::Type::ChangeVolume:
                    if (music)
                    {
                        music->setVolume(fadeVolumeMultiplier *
                                         int(double(musicVolume) * userMusicVolume / 100.0));
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


    bool AudioEngine::PlayMusic(const std::string& ID, uint64_t timePos)
    {
        // Unlike sounds, there's only one music playing at a time
        // Partly due to SDL Mixer implementation
        if (GetMusicLib().find(ID) == GetMusicLib().end())
        {
            Game::LogHandler::Log("Couldn't play music " + ID,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        if (music && music->getStatus() == sf::Music::Playing)
        {
            AddAction(MusicAction::Type::Stop);
        }
        AddAction(MusicAction::Type::Start, ID, {timePos});

        // Run an update to init actions
        Update();

        return true;
    }

    bool AudioEngine::StopMusic()
    {
        if (!music || music->getStatus() == sf::Music::Stopped)
        {
            return false;
        }
        AddAction(MusicAction::Type::Stop);
        return true;
    }

    void AudioEngine::HaltEngine()
    {
        std::queue<AudioEngine::MusicAction>().swap(actionQueue); // Empties queue
        musicID = "";
        music->stop();
        sounds.clear();
    }

    void AudioEngine::SetUserMusicVolume(double volume)
    {
        userMusicVolume = (uint8_t)Utility::ClampValue(volume, 0.0, 100.0);
        AddAction(MusicAction::Type::ChangeVolume, "");
    }

    void AudioEngine::SetUserSoundVolume(double volume)
    {
        userSoundVolume = (uint8_t)Utility::ClampValue(volume, 0.0, 100.0);
        for (auto& [id, sound] : sounds)
            sound->setVolume(soundVolume / 100.f * userSoundVolume));
    }

    void AudioEngine::SetMusicVolume(double volume)
    {
        musicVolume = (uint8_t)Utility::ClampValue(volume, 0.0, 100.0);
        AddAction(MusicAction::Type::ChangeVolume, "");
    }

    void AudioEngine::SetSoundVolume(double volume)
    {
        soundVolume = (uint8_t)Utility::ClampValue(volume, 0.0, 100.0);
        for (auto& [id, sound] : sounds)
            sound->setVolume(soundVolume / 100.f * userSoundVolume);
    }

    bool AudioEngine::SetLoopSection(const std::string& section)
    {
        if (GetMusicLib().find(musicID) == GetMusicLib().end())
        {
            return false;
        }
        auto& sectionList = SearchMusicLib(musicID).sectionList;
        if (sectionList.find(section) == sectionList.end())
        {
            Game::LogHandler::Log("Couldn't set loop section " + section +
                                      ". Current music playing: \"" + musicID + "\"",
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }
        AddAction(MusicAction::Type::SetLoopSection, section);
        return true;
    }

    bool AudioEngine::SeekToSection(const std::string& section)
    {
        AddAction(MusicAction::Type::Seek, section);
        return true;
    }

    bool AudioEngine::IsEngineIdle()
    {
        Update(); // Improves accuracy
        return actionQueue.size() == 0 &&
               (!music || music->getStatus() == sf::Music::Stopped) && sounds.size() == 0;
    }
} // namespace Game