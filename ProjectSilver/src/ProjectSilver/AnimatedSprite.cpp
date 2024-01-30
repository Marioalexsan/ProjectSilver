module;

#include <ProjectSilver/PCHeader.hpp>

module ProjectSilver.AnimatedSprite;
import ProjectSilver.GraphicsEngine;

namespace Game
{
    AnimatedSprite::AnimatedSprite() :
    finished(false),
    framesPerCollumn(0),
    framesPerRow(0),
    totalFrames(0),
    updatesPerFrame(0),
    currentFrame(-1),
    direction(1),
    accumulatedUpdates(0),
    mode(LoopMode::NormalLoop)
    {
    }

    void AnimatedSprite::SetAnimationInfo(int      updatesPerFrame,
                                          int      framesPerRow,
                                          int      framesPerCollumn,
                                          LoopMode mode)
    {
        this->updatesPerFrame  = updatesPerFrame;
        this->framesPerCollumn = framesPerCollumn;
        this->framesPerRow     = framesPerRow;
        totalFrames            = framesPerCollumn * framesPerRow;
        this->mode             = mode;
    }

    void AnimatedSprite::SetFrame(int frame)
    {
        currentFrame       = frame;
        accumulatedUpdates = 0;
        if (currentFrame >= totalFrames)
        {
            currentFrame = totalFrames - 1;
        }
    }

    void AnimatedSprite::Restart()
    {
        direction          = 1;
        currentFrame       = -1;
        accumulatedUpdates = 0;
        finished           = false;
    }

    void AnimatedSprite::Update(int frameUpdates)
    {
        accumulatedUpdates++;
        if (accumulatedUpdates >= updatesPerFrame)
        {
            accumulatedUpdates -= updatesPerFrame;

            switch (mode)
            {
                case LoopMode::PingPongLoop:
                {
                    if (direction == 1)
                    {
                        currentFrame++;
                        if (currentFrame >= totalFrames)
                        {
                            currentFrame -= 2;
                            direction = -1;
                        }
                    }
                    else if (direction == -1)
                    {
                        currentFrame--;
                        if (currentFrame < 0)
                        {
                            currentFrame += 2;
                            direction = 1;
                        }
                    }
                }
                break;
                case LoopMode::NormalLoop:
                {
                    currentFrame++;
                    if (currentFrame >= totalFrames)
                    {
                        currentFrame = 0;
                    }
                }
                break;
                case LoopMode::PlayOnce:
                {
                    if (direction != 0)
                    {
                        currentFrame++;
                        if (currentFrame >= totalFrames)
                        {
                            currentFrame = totalFrames - 1;
                            direction    = 0;
                            finished     = true;
                        }
                    }
                }
                break;
            }
        }

        currentFrame = Utility::ClampValue(currentFrame, 0, totalFrames - 1);
    }

    void AnimatedSprite::Draw()
    {
        using namespace Game::Utility;
        if (texture == nullptr || currentFrame < 0 || currentFrame >= totalFrames)
        {
            return;
        }
        auto size = texture->getSize();

        int frameWidth  = size.x / framesPerRow;
        int frameHeight = size.y / framesPerCollumn;

        int collumn = currentFrame % framesPerRow;
        int row     = currentFrame / framesPerRow;

        sf::Sprite sprite(*texture,
                          sf::IntRect{sf::Vector2i{frameWidth * collumn, frameHeight * row},
                                      sf::Vector2i{frameWidth, frameHeight}});

        sprite.setPosition(
            sf::Vector2f{(float)transform->position.x, (float)transform->position.y});
        sprite.setOrigin(sf::Vector2f{(float)transform->center.x, (float)transform->center.y});
        sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
        sprite.setRotation(sf::degrees(transform->direction));

        GraphicsEngine::Window->draw(sprite);
    }

} // namespace Game