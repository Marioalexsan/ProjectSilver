#ifndef ASSET_HEADER
#define ASSET_HEADER

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include "PCHeader.h"
#include "MiscUtility.h"

namespace Game {
	class AssetManager {
	public:
		#pragma region Audio Resources

		// Music section
		struct Section {
			uint64_t	start;
			uint64_t	end;
		};

		// Holds the data about a music entry (streamed audio)
		struct MusicData {
			map<string, Section>	sectionList;

			string		path;
			double		baseVolume;
			Mix_Music* samples;

			uint64_t	duration; // In Milliseconds

			MusicData();
			MusicData(const string&, double, Mix_Music*, uint64_t);
		};

		// Holds the data about a sound entry (streamed music)
		struct SoundData {
			string		path;
			double		baseVolume;
			Mix_Chunk* samples;

			SoundData();
			SoundData(const string&, double, Mix_Chunk*);
		};

		#pragma endregion

		#pragma region Graphics Resources

		struct TextureData {
			string path;
			SDL_Texture* texture;

			TextureData();
			TextureData(const string&, SDL_Texture*);
		};

		// Only supports single page sets, latin
		struct SpriteFontData {
			struct CharData {
				int x;
				int y;
				int width;
				int height;
				int xoffset;
				int yoffset;
				int xadvance;
				int channel;
			};

			string face;
			string charset;
			string file;

			map<string, int> arbitraryValues;

			int padding[4];
			int spacing[2];

			map<char, CharData> charLibrary;

			//map<string, int> arbitraryValues;
		};

		#pragma endregion

	private:
		// Currently loaded audio entries
		map<string, MusicData> musicLibrary;
		map<string, SoundData> soundLibrary;
		map<string, TextureData> textureLibrary;
		map <string, pair<SpriteFontData, TextureData>> fontLibrary;

	public:
		AssetManager();
		~AssetManager();

		bool LoadSound(const string& ID, const string& path);
		bool LoadMusic(const string& ID, const string& path, uint64_t duration = 0);
		bool LoadTexture(const string& ID, const string& path);
		bool LoadSpriteFont(const string& ID, const string& texPath, const string& dataPath);
		
		bool UnloadSound(const string& ID); // WARNING: Do not call UnloadSound if it is still playing!
		bool UnloadMusic(const string& ID);
		bool UnloadTexture(const string& ID);
		bool UnloadSpriteFont(const string& ID);

		bool LoadMusicSections(const string& ID, const map<string, Section>& sectionList);

		const map<string, MusicData>& GetMusicLibrary();
		const map<string, SoundData>& GetSoundLibrary();
		const map<string, TextureData>& GetTextureLibrary();
		const map<string, pair<SpriteFontData, TextureData>>& GetFontLibrary();
	};
}

#endif