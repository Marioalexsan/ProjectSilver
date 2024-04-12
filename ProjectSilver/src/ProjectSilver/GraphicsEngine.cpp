#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/GraphicsEngine.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    std::unique_ptr<sf::RenderWindow> GraphicsEngine::Window = nullptr;
    std::unique_ptr<sf::RenderTexture> GraphicsEngine::World  = nullptr;

    int GraphicsEngine::windowWidth  = 0;
    int GraphicsEngine::windowHeight = 0;
    int GraphicsEngine::renderWidth  = 0;
    int GraphicsEngine::renderHeight = 0;

    double GraphicsEngine::gameWinFadeout = 0.0;
    double GraphicsEngine::gameLoseFadeout = 0.0;

    Vector2 GraphicsEngine::cameraPosition = {0.0, 0.0};

    GraphicsEngine::GraphicsEngine() : currentID(1)
    {
        windowWidth  = 1280;
        windowHeight = 720;
        renderWidth  = 1280;
        renderHeight = 720;
        //SetDisplayMode({1280, 720, false});
    }

    GraphicsEngine::~GraphicsEngine()
    {
    }

    Vector2 GraphicsEngine::GetWindowSize()
    {
        auto [x, y] = Window->getSize();
        return {double(x), double(y)};
    }

    bool GraphicsEngine::SetDisplayMode(VideoMode mode)
    {
        auto style = mode.fullscreen ? sf::Style::None : sf::Style::Titlebar | sf::Style::Close;
        auto state = mode.fullscreen ? sf::State::Fullscreen : sf::State::Windowed;

        if (fullscreen || mode.fullscreen)
        {
            // Use create if we were in fullscreen, or are going into fullscreen
            Window->setVisible(false);
            Window->create(sf::VideoMode{{(std::uint32_t)mode.width,
                                          (std::uint32_t)mode.height}},
                           "ProjectSilver",
                           style,
                           state);
        }
        else
        {
            Window->setSize({(std::uint32_t)mode.width, (std::uint32_t)mode.height});
        }

        Window->setMouseCursorVisible(false);
        renderWidth = mode.width;
        renderHeight = mode.height;
        fullscreen = mode.fullscreen;
        return true;
    }

    void GraphicsEngine::SetStandardViewport()
    {
        // TODO
    }

    uint64_t GraphicsEngine::NextID()
    {
        uint64_t val = currentID++;
        currentID    = currentID == 0 ? 1 : currentID;
        return val;
    }

    uint64_t GraphicsEngine::AddDrawable(Drawable* element)
    {
        uint64_t ID         = NextID();
        drawableLibrary[ID] = element;
        return ID;
    }

    bool GraphicsEngine::RemoveDrawable(uint64_t ID)
    {
        if (drawableLibrary.find(ID) == drawableLibrary.end())
        {
            return false;
        }
        drawableLibrary.erase(ID);
        return true;
    }

    void GraphicsEngine::ClearDrawables()
    {
        drawableLibrary.clear();
    }

    void GraphicsEngine::RenderAll()
    {
        if (!World || World->getSize() != Window->getSize())
        {
            World = std::make_unique<sf::RenderTexture>();
            (void)World->create(Window->getSize());
        }

        sf::View worldView = World->getView();
        worldView.setSize({ResolutionTargetWidth, ResolutionTargetHeight});
        World->setView(worldView);

        World->clear(sf::Color::Black);

        std::map<int, std::vector<Drawable*>> layeredDrawables;

        for (auto& elem : drawableLibrary)
        {
            layeredDrawables[elem.second->GetLayer()].push_back(elem.second);
        }
        for (auto& elem : layeredDrawables)
        {
            for (auto& draw : elem.second)
            {
                UseCamera(draw->GetRelativeToCamera());
                draw->Draw();
            }
        }

        World->display();

        sf::View windowView = Window->getView();
        windowView.setSize((sf::Vector2f)Window->getSize());
        windowView.setCenter((sf::Vector2f)Window->getSize() / 2.f);
        Window->setView(windowView);

        Window->clear(sf::Color::Black);

        sf::Sprite worldSprite(World->getTexture());

        if (gameWinFadeout != 0)
        {
            auto color = sf::Color::White;
            auto factor = (100 - gameWinFadeout) / 100;
            color.a     = (uint8_t)std::clamp(255 * factor, 0.0, 255.0);
            worldSprite.setColor(color);
        }
        else if (gameLoseFadeout != 0)
        {
            auto color  = sf::Color::White;
            auto factor = (100 - gameLoseFadeout) / 100;

            float avg = (color.r + color.g + color.b) / 3;
            color.r = (uint8_t)std::clamp(factor * color.r + avg * (1 - factor), 0.0, 255.0);
            color.g = (uint8_t)std::clamp(factor * color.g + avg * (1 - factor), 0.0, 255.0);
            color.b = (uint8_t)std::clamp(factor * color.b + avg * (1 - factor), 0.0, 255.0);
            color.a = (uint8_t)std::clamp(255 * (factor * 0.5 + 0.5), 0.0, 255.0);

            worldSprite.setColor(color);
        }

        Window->draw(worldSprite);
        Window->display();
    }

    void GraphicsEngine::EmptyRender()
    {
        Window->clear(sf::Color::Black);
        Window->display();
    }

    void GraphicsEngine::SetCameraPosition(Vector2 position)
    {
        cameraPosition = position;
    }

    Vector2 GraphicsEngine::GetCameraPosition()
    {
        return cameraPosition;
    }

    void GraphicsEngine::CenterCameraOn(Vector2 position)
    {
        cameraPosition = position - Vector2(960.0, 540.0);
    }

    void GraphicsEngine::PushCamera(Vector2 pushAmount)
    {
        cameraPosition += pushAmount;
    }

    void GraphicsEngine::UseCamera(bool useCamera)
    {
        if (useCamera)
        {
            sf::View view = World->getView();
            view.setCenter(sf::Vector2f{float(cameraPosition.x), float(cameraPosition.y)} +
                           sf::Vector2f(960.f, 540.f));
            World->setView(view);
        }
        else
        {
            sf::View view = World->getView();
            view.setCenter({960, 540});
            World->setView(view);
        }
    }
} // namespace Game