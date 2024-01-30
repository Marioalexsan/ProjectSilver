module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.AudioEngine;
import ProjectSilver.AssetManager;
import ProjectSilver.MiscUtility;

export namespace Game
{

    // Can't actually use callbacks due to a race condition between main thread and audio thread


    /** Audio Engine Class
	 * Features:
		* Plays sound - short duration audio that is loaded in memory
			* Up to maxChannels sounds at the same time
		* Plays music - long duration audio that is streamed from disk
			* One at a time
			* Features section looping and seeking
	*/
    class AudioEngine
    {
    public:
        static const int maxDistance           = 1000; // UNUSED
        static const int sectionSwitchFadeTime = 150;
        static const int musicSwitchFadeTime   = 720;

    private:
        // Music Actions are used to tell the engine how to play music
        // This allows fade in and fade out to work easily, and also guarantees sequentiality of actions
        // This means that Start -> Seek will correctly start and then seek a music,
        // Even if a music might not be "playing" directly after Start (SDL audio in in a different thread, after all)

        // param - used for music ID, mostly
        // extraparams - other properties which may be passed in the future (volume? custom fade time?)
        // useFade - true by default. If false, Update() will not use fading for that action
        struct MusicAction
        {
            enum class Type
            {
                None,
                Stop,
                Start,
                Seek,
                SetLoopSection,
                ChangeVolume
            };
            enum class SubType
            {
                None,
                FadeOutSection,
                FadeInSection
            };
            Type                  type;
            SubType               subType;
            std::string           param;
            std::vector<uint64_t> extraParams;
            bool                  useFade;
        };

        enum class SoundEffect
        {
            Distance // UNIMPLEMENTED. There's little reason to add it now though.
        };

        // Current State Variables
        // Allows tracking of elapsed time, and assigning numerical IDs to music / sound instances
        // While a generous presumption, the IDs returned via NextID() should be unique
        // (unless you make more than MaxInt64 sound instances in a program launch...)
        sf::Clock stopwatch;
        uint64_t  currentID;

        // Action queue used by Update()
        std::queue<MusicAction> actionQueue;

        // Stores the sound and music instances. For sounds, the map's key is the numerical ID
        std::map<uint64_t, std::unique_ptr<sf::Sound>> sounds;
        std::map<uint64_t, std::string>                soundDataIDs;
        sf::Music*                                     music = nullptr;
        std::string                                    musicID;

        // Stats

        bool     fadeToSilence        = false;
        float    fadeVolumeMultiplier = 0.0f;
        sf::Time fadeDuration         = sf::Time::Zero;

        // General Volume
        uint8_t musicVolume;
        uint8_t soundVolume;

        uint8_t userMusicVolume;
        uint8_t userSoundVolume;

        uint64_t NextID();

        // Private function used for readability
        void AddAction(MusicAction::Type,
                       const std::string&           = "",
                       const std::vector<uint64_t>& = {0},
                       bool                         = true);

        // Cannot be inlined due to a circular dependency (either that or I suck at code)
        const std::map<std::string, AssetManager::MusicData>& GetMusicLib();
        const std::map<std::string, AssetManager::SoundData>& GetSoundLib();

        const AssetManager::MusicData& SearchMusicLib(const std::string& ID);
        const AssetManager::SoundData& SearchSoundLib(const std::string& ID);

    public:
        AudioEngine();
        ~AudioEngine();

        uint64_t PlaySound(const std::string& ID, const Vector2 pos = {});
        bool     StopSound(const std::string& ID);
        bool     StopSound(uint64_t ID);

        bool PlayMusic(const std::string& ID, uint64_t timePos = 0);
        bool StopMusic();

        void HaltEngine();

        // This function is important
        void Update();

        inline int GetSoundCount()
        {
            return sounds.size();
        }

        inline double GetMusicVolume()
        {
            return musicVolume;
        }
        inline double GetSoundVolume()
        {
            return soundVolume;
        }

        inline uint8_t GetUserMusicVolume()
        {
            return userMusicVolume;
        }
        inline uint8_t GetUserSoundVolume()
        {
            return userSoundVolume;
        }

        void SetUserMusicVolume(double volume);
        void SetUserSoundVolume(double volume);

        void SetMusicVolume(double volume);
        void SetSoundVolume(double volume);

        inline uint64_t GetMusicPosition()
        {
            return music ? music->getPlayingOffset().asMilliseconds() : 0;
        }

        bool SetLoopSection(const std::string& section);
        bool SeekToSection(const std::string& section);

        bool IsEngineIdle();
    };
} // namespace Game
