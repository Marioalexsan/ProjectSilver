#ifndef AUDIOENGINE_HEADER
#define AUDIOENGINE_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"

class AssetManager;

using std::uint64_t;

namespace Game {

	// Can't actually use callbacks due to a race condition between main thread and audio thread


	/** Audio Engine Class
	 * Features:
		* Plays sound - short duration audio that is loaded in memory
			* Up to maxChannels sounds at the same time
		* Plays music - long duration audio that is streamed from disk
			* One at a time
			* Features section looping and seeking
	*/
	class AudioEngine {
	public:
		static const int maxChannels = 64;
		static const int maxDistance = 1000;
		static const int defaultFadeTime = 400;



	private:
		// A sound instance that's currently playing
		struct Sound {
			string		dataID;
			Vector2		worldPos;
			int			channel;
		};

		// A music instance that's currently playing
		// The engine only uses one such instance, but the struct is used for organization purposes
		struct Music {
			string		dataID;
			uint64_t	timePos;
			uint64_t	loopStart;
			uint64_t	loopEnd;
		};

		// Music Actions are used to tell the engine how to play music
		// This allows fade in and fade out to work easily, and also guarantees sequentiality of actions
		// This means that Start -> Seek will correctly start and then seek a music,
		// Even if a music might not be "playing" directly after Start (SDL audio in in a different thread, after all)

		// param - used for music ID, mostly
		// extraparams - other properties which may be passed in the future (volume? custom fade time?)
		// useFade - true by default. If false, Update() will not use fading for that action
		struct MusicAction {
			enum class Type {
				None,
				Stop,
				Start,
				Seek,
				SetLoopSection
			};
			enum class SubType {
				None,
				FadeOutSection,
				FadeInSection
			};
			Type				type;
			SubType				subType;
			string				param;
			vector<uint64_t>	extraParams;
			bool				useFade;
		};

		enum class SoundEffect {
			Distance
		};

		// Current State Variables
		// Allows tracking of elapsed time, and assigning numerical IDs to music / sound instances
		// While a generous presumption, the IDs returned via NextID() should be unique
		// (unless you make more than MaxInt64 sound instances in a program launch...)
		uint32_t currentTicks;
		uint64_t currentID;
		
		// Action queue used by Update()
		queue<MusicAction> actionQueue;

		

		// Stores the ID of the sound instances playing. 0 means the channel is free
		array<uint64_t, maxChannels> channels;

		// Stores the sound and music instances. For sounds, the map's key is the numerical ID
		map<uint64_t, Sound> sounds;
		Music music;

		// Stats
		int soundCount;

		// General Volume
		uint8_t musicVolume;
		uint8_t soundVolume;

		uint64_t	NextID();

		// Private function used for readability
		void		AddAction(MusicAction::Type, const string & = "", const vector<uint64_t>& = { 0 }, bool = true);

		const map<string, AssetManager::MusicData>& GetMusicLib();
		const map<string, AssetManager::SoundData>& GetSoundLib();

		const AssetManager::MusicData& SearchMusicLib(const string& ID);
		const AssetManager::SoundData& SearchSoundLib(const string& ID);
	public:

		AudioEngine();
		~AudioEngine();

		uint64_t PlaySound(const string& ID, const Vector2 pos = {});
		bool StopSound(const string& ID);
		bool StopSound(uint64_t ID);

		bool PlayMusic(const string& ID, uint64_t timePos = 0);
		bool StopMusic();

		void HaltEngine();

		void Update();
		int GetSoundCount();



		uint64_t GetMusicPosition();
		bool IsMusicPlaying();

		bool SetLoopSection(const string& section);
		bool SeekToSection(const string& section);

		bool IsEngineIdle();
	};
}

#endif