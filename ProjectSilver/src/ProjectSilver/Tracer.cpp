#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Tracer.hpp>

namespace Game
{
    Tracer::Tracer(Vector2 start, Vector2 end) :
    fadeCounter(0),
    targetFadeCounter(0),
    start(start),
    end(end),
    body(nullptr),
    endCap(nullptr)
    {
        Vector2 directionVector = end - start;
        double  speedPower      = rand() % 40 / 100.0 + 0.6;
        double  forwardAngle    = directionVector.Angle();
        double  sideAngle       = Utility::ScrollValue(directionVector.Angle() - 90.0 +
                                                    (rand() % 2 * 180.0),
                                                0.0,
                                                360.0);
        forwardSpeed            = Vector2::NormalVector(forwardAngle) * speedPower *
                       (0.2 + rand() % 2 / 10.0);
        sideSpeed = Vector2::NormalVector(sideAngle) * speedPower;
    }

    Tracer::~Tracer()
    {
    }

    void Tracer::SetBodyTexture(const std::string& texture)
    {
        auto& lib = Globals::Assets().GetTextureLibrary();
        if (lib.find(texture) == lib.end())
        {
            //Error
            return;
        }
        this->body = lib.at(texture).texture.get();
    }

    void Tracer::SetCapTexture(const std::string& texture)
    {
        auto& lib = Globals::Assets().GetTextureLibrary();
        if (lib.find(texture) == lib.end())
        {
            //Error
            return;
        }
        this->endCap = lib.at(texture).texture.get();
    }

    void Tracer::Update()
    {
        if (timeToLive <= 0)
        {
            return;
        }

        bool   fading     = fadeCounter <= targetFadeCounter / 2;
        double fadeFactor = double(fadeCounter) / (targetFadeCounter / 2);

        if (fadeCounter > 0)
        {
            fadeCounter--;
            if (fading && targetFadeCounter > 0)
            {
                SetAlpha(uint8_t(GetAlpha() * fadeFactor));
            }
        }
        if (fading)
        {
            double speedFactor = 1.6 - 1.5 * fadeFactor * fadeFactor;
            start += (forwardSpeed + sideSpeed) * speedFactor;
            end += (forwardSpeed + sideSpeed) * speedFactor;
        }
        else
        {
            start += (forwardSpeed + sideSpeed) * 0.1;
            end += (forwardSpeed + sideSpeed) * 0.1;
        }
        Effect::Update();
    }

    void Tracer::Draw()
    {
        using namespace Game::Utility;
        if (body == nullptr)
        {
            return;
        }

        // Body render
        // Cap render is UNIMPLEMENTED for now
        // AKA this class acts as an overglorified Sprite until that is done

        sf::Sprite sprite(*body);

        sprite.setPosition(sf::Vector2f{(float)start.x, (float)start.y});
        sprite.setScale(sf::Vector2f{float((end - start).Length()), 1});
        sprite.setOrigin(sf::Vector2f{0, float(body->getSize().y / 2)});
        sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
        sprite.setRotation(sf::degrees((end - start).Angle() - 90.0));

        GraphicsEngine::World->draw(sprite);
    }
} // namespace Game