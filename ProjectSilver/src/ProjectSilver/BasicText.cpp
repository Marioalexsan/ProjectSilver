#include <ProjectSilver/BasicText.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/GraphicsEngine.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    BasicText::BasicText() :
    texture(nullptr),
    fontID(""),
    left(0),
    right(100),
    top(0),
    bottom(100),
    type(TextRenderType::ContinuousLeft)
    {
    }

    BasicText::BasicText(const std::string&        fontID,
                         const std::string&        text,
                         BasicText::TextRenderType type) :
    text(text),
    type(type),
    left(0),
    right(100),
    top(0),
    bottom(100)
    {
        SetFont(fontID);
    }

    int CalculateTextLength(const AssetManager::SpriteFontData* fontData,
                            const std::string&                  text)
    {
        if (text.length() == 0)
        {
            return 0;
        }

        int pixels = 0;

        pixels += fontData->charLibrary.at(text[0]).xoffset;

        for (int i = 0; i < text.length(); i++)
        {
            pixels += fontData->charLibrary.at(text[i]).xadvance;
        }
        pixels += fontData->charLibrary.at(text[text.length() - 1]).width;
        pixels -= fontData->charLibrary.at(text[text.length() - 1]).xadvance;
        pixels -= fontData->charLibrary.at(text[text.length() - 1]).xoffset;
        return pixels;
    }

    void BasicText::Draw()
    {
        auto& lib = Globals::Assets().GetFontLibrary();

        if (lib.find(fontID) == lib.end())
        {
            return;
        }

        auto& data = lib.at(fontID).first;

        if (texture == nullptr)
        {
            // Bad draw
            return;
        }

        if (text.length() == 0)
        {
            // Nothing to draw
            return;
        }

        int lineHeight = data.arbitraryValues.at("lineHeight");

        switch (type)
        {
            case TextRenderType::ContinuousLeft:
            case TextRenderType::ContinuousRight:
            case TextRenderType::ContinuousCentered:
            {
                int currentX = 0;
                int wordPos  = 0;

                int orientationOffset = 0;

                if (type == TextRenderType::ContinuousCentered)
                {
                    orientationOffset = CalculateTextLength(&data, text) / 2;
                }
                else if (type == TextRenderType::ContinuousRight)
                {
                    orientationOffset = CalculateTextLength(&data, text);
                }

                while (wordPos < text.length())
                {
                    auto& info = data.charLibrary.at(text[wordPos]);


                    sf::Sprite sprite(*texture, sf::IntRect{sf::Vector2i{info.x, info.y}, sf::Vector2i{info.width, info.height}});

                    sprite.setPosition(sf::Vector2f{(float)transform->position.x + currentX + info.xoffset - orientationOffset,
                                                    (float)transform->position.y + info.yoffset});
                    sprite.setOrigin(sf::Vector2f{0, 0});
                    sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
                    sprite.setRotation(sf::degrees(transform->direction));

                    GraphicsEngine::Window->draw(sprite);

                    wordPos++;
                    currentX += info.xadvance;
                }
            }
            break;
            default:
                return;
        }
    }


    void BasicText::SetFont(const std::string& ID)
    {
        AssetManager& manager = Globals::Assets();
        auto&         lib     = manager.GetFontLibrary();
        if (lib.find(ID) == lib.end())
        {
            //Error
            return;
        }
        this->texture = lib.at(ID).second.texture.get();
        this->fontID  = ID;
    }

    void BasicText::SetText(const std::string& text)
    {
        this->text = text;
    }

} // namespace Game
