module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.AssetManager;
import ProjectSilver.MiscUtility;

export namespace Game
{
    class AssetManager
    {
    public:
        // Music section
        struct Section
        {
            uint64_t start;
            uint64_t end;
        };

        // Holds the data about a music entry (streamed audio)
        struct MusicData
        {
            std::map<std::string, Section> sectionList;

            std::string                path;
            double                     baseVolume;
            std::unique_ptr<sf::Music> music;

            uint64_t duration; // In Milliseconds
        };

        // Holds the data about a sound entry (streamed music)
        struct SoundData
        {
            std::string                      path;
            double                           baseVolume;
            std::unique_ptr<sf::SoundBuffer> samples;
        };

        struct TextureData
        {
            std::string                  path;
            std::unique_ptr<sf::Texture> texture;
        };

        // Only supports single page sets, latin
        struct SpriteFontData
        {
            struct CharData
            {
                int x;
                int y;
                int width;
                int height;
                int xoffset;
                int yoffset;
                int xadvance;
                int channel;
            };

            std::string face;
            std::string charset;
            std::string file;

            std::map<std::string, int> arbitraryValues;

            int padding[4];
            int spacing[2];

            std::map<char, CharData> charLibrary;

            //map<string, int> arbitraryValues;
        };

#pragma endregion

    private:
        // Currently loaded audio entries
        std::map<std::string, MusicData>                              musicLibrary;
        std::map<std::string, SoundData>                              soundLibrary;
        std::map<std::string, TextureData>                            textureLibrary;
        std::map<std::string, std::pair<SpriteFontData, TextureData>> fontLibrary;

    public:
        AssetManager();
        ~AssetManager();

        // Load and Unload functions return true on success, false if the operation failed for whatever reason

        bool LoadSound(const std::string& ID, const std::string& path);
        bool LoadMusic(const std::string& ID, const std::string& path, uint64_t duration = 0);
        bool LoadTexture(const std::string& ID, const std::string& path);
        bool LoadSpriteFont(const std::string& ID,
                            const std::string& texPath,
                            const std::string& dataPath);

        bool LoadMusicSections(const std::string&                    ID,
                               const std::map<std::string, Section>& sectionList);

        bool UnloadSound(const std::string& ID); // WARNING: Do not call UnloadSound if it is still playing!
        bool UnloadMusic(const std::string& ID);
        bool UnloadTexture(const std::string& ID);
        bool UnloadSpriteFont(const std::string& ID);

        inline const std::map<std::string, MusicData>& GetMusicLibrary()
        {
            return musicLibrary;
        }
        inline const std::map<std::string, SoundData>& GetSoundLibrary()
        {
            return soundLibrary;
        }
        inline const std::map<std::string, TextureData>& GetTextureLibrary()
        {
            return textureLibrary;
        }
        inline const std::map<std::string, std::pair<SpriteFontData, TextureData>>& GetFontLibrary()
        {
            return fontLibrary;
        }
    };
} // namespace Game
