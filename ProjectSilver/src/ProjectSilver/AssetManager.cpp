#include <ProjectSilver/AssetManager.hpp>
#include <ProjectSilver/GraphicsEngine.hpp>
#include <ProjectSilver/LogHandler.hpp>
#include <ProjectSilver/PCHeader.hpp>

#pragma region Helper Functions

void DiscardWord(std::ifstream& file, char delim = ' ')
{
    file >> std::ws;
    file.ignore(std::numeric_limits<std::streamsize>::max(), delim);
}

void ReadWordPair(std::ifstream& file, std::pair<std::string, std::string>& dst, char delim = '\"')
{
    file >> std::ws;
    getline(file, dst.first, '=');
    file.ignore();
    getline(file, dst.second, delim);
}

void ReadField(std::ifstream& file, std::pair<std::string, int>& dst, char delim = ' ')
{
    file >> std::ws;
    getline(file, dst.first, '=');
    std::string temp;
    getline(file, temp, delim);
    dst.second = std::stoi(temp);
}

void ReadPadding(std::ifstream& file, int* padArray, char delim = ' ')
{
    file >> std::ws;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '=');
    std::string temp;
    for (int i = 0; i < 3; i++)
    {
        getline(file, temp, ',');
        padArray[i] = std::stoi(temp);
    }
    getline(file, temp, delim);
    padArray[3] = std::stoi(temp);
}

void ReadSpacing(std::ifstream& file, int* spaceArray, char delim = ' ')
{
    file >> std::ws;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '=');
    std::string temp;
    getline(file, temp, ',');
    spaceArray[0] = std::stoi(temp);
    getline(file, temp, delim);
    spaceArray[1] = std::stoi(temp);
}

#pragma endregion

namespace Game
{
    AssetManager::AssetManager()
    {
    }

    AssetManager::~AssetManager()
    {
        while (musicLibrary.size() > 0)
        {
            UnloadMusic(musicLibrary.begin()->first);
        }
        while (soundLibrary.size() > 0)
        {
            UnloadSound(soundLibrary.begin()->first);
        }
        while (textureLibrary.size() > 0)
        {
            UnloadTexture(textureLibrary.begin()->first);
        }
        while (fontLibrary.size() > 0)
        {
            UnloadSpriteFont(fontLibrary.begin()->first);
        }
    }

    bool AssetManager::LoadSound(const std::string& ID, const std::string& path)
    {
        if (soundLibrary.find(ID) != soundLibrary.end())
        {
            return false;
        }

        auto samples = std::make_unique<sf::SoundBuffer>();

        if (!samples->loadFromFile(path))
        {
            Game::LogHandler::Log("Couldn't load sound " + path + ", ID " + ID,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        soundLibrary[ID] = {path, 100.0, std::move(samples)};
        return true;
    }

    bool AssetManager::LoadMusic(const std::string& ID, const std::string& path, uint64_t duration)
    {
        if (musicLibrary.find(ID) != musicLibrary.end())
        {
            return false;
        }

        auto samples = std::make_unique<sf::Music>();
        if (!samples->openFromFile(path))
        {
            Game::LogHandler::Log("Couldn't load music " + path + ", ID " + ID,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        musicLibrary[ID] = {{}, path, 100.0, std::move(samples), duration};
        return true;
    }

    bool AssetManager::UnloadSound(const std::string& ID)
    {
        if (soundLibrary.find(ID) == soundLibrary.end())
        {
            return false;
        }

        soundLibrary.erase(ID);
        return true;
    }

    bool AssetManager::UnloadMusic(const std::string& ID)
    {
        if (musicLibrary.find(ID) == musicLibrary.end())
        {
            return false;
        }

        musicLibrary.erase(ID);
        return true;
    }

    bool AssetManager::LoadMusicSections(const std::string& ID,
                                         const std::map<std::string, Section>& sectionList)
    {
        if (musicLibrary.find(ID) == musicLibrary.end())
        {
            Game::LogHandler::Log("Tried to load sections for unknown music " + ID,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        // Just copy it LOL
        // Maybe setup a check for invalid sections in the future
        musicLibrary[ID].sectionList = sectionList;
        return true;
    }

    bool AssetManager::LoadTexture(const std::string& ID, const std::string& path)
    {
        if (textureLibrary.find(ID) != textureLibrary.end())
        {
            return false;
        }

        // Try load
        auto texture = std::make_unique<sf::Texture>();

        if (!texture->loadFromFile(path))
        {
            Game::LogHandler::Log("Failed to create texture for image " + path + ", ID " + ID,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        texture->setSmooth(true);

        textureLibrary[ID] = {path, std::move(texture)};
        return true;
    }

    bool AssetManager::LoadSpriteFont(const std::string& ID,
                                      const std::string& texPath,
                                      const std::string& dataPath)
    {
        if (fontLibrary.find(ID) != fontLibrary.end())
        {
            return false;
        }

        SpriteFontData data;

        std::ifstream fontData(dataPath);
        if (!fontData.good())
        {
            return false;
        }
        std::map<std::string, int> arbitraries;

        int section = 0;
        try
        {
            using std::getline;
            std::pair<std::string, std::string> strpair;
            std::pair<std::string, int>         field;

            // Info section
            DiscardWord(fontData);
            ReadWordPair(fontData, strpair);
            data.face = strpair.second;

            for (int i = 0; i < 3; i++)
            {
                ReadField(fontData, field);
                data.arbitraryValues[field.first] = field.second;
            }

            ReadWordPair(fontData, strpair);
            data.charset = strpair.second;

            for (int i = 0; i < 4; i++)
            {
                ReadField(fontData, field);
                data.arbitraryValues[field.first] = field.second;
            }

            ReadPadding(fontData, data.padding);
            ReadSpacing(fontData, data.spacing);

            ReadField(fontData, field, '\n');
            data.arbitraryValues[field.first] = field.second;

            // Common section
            DiscardWord(fontData);

            for (int i = 0; i < 9; i++)
            {
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

            for (int i = 0; i < count; i++)
            {
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

        } catch (...)
        {
            fontData.close();
            Game::LogHandler::Log("An exception happened while reading font data, ID " + ID,
                                  Game::LogHandler::MessageType::Warn);
            Game::LogHandler::Log("Data Path: " + dataPath,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        fontData.close();

        auto texture = std::make_unique<sf::Texture>();

        if (!texture->loadFromFile(texPath))
        {
            Game::LogHandler::Log("Failed to create texture for font, ID " + ID,
                                  Game::LogHandler::MessageType::Warn);
            Game::LogHandler::Log("Texture Path: " + texPath,
                                  Game::LogHandler::MessageType::Warn);
            return false;
        }

        texture->setSmooth(true);

        fontLibrary[ID] = {data, {texPath, std::move(texture)}};
        return true;
    }

    bool AssetManager::UnloadTexture(const std::string& ID)
    {
        if (textureLibrary.find(ID) == textureLibrary.end())
        {
            return false;
        }

        textureLibrary.erase(ID);
        return true;
    }

    bool AssetManager::UnloadSpriteFont(const std::string& ID)
    {
        if (fontLibrary.find(ID) == fontLibrary.end())
        {
            return false;
        }

        fontLibrary.erase(ID);
        return true;
    }

#pragma endregion
} // namespace Game