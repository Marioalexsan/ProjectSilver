#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/RenderComponent.hpp>

namespace Game
{
    RenderComponent::RenderComponent() :
    currentAnimation(""),
    defaultAnimation(""),
    switchToDefaultOnEnd(false),
    parsedStart(false),
    parsedEnd(false)
    {
    }

    bool RenderComponent::IsCurrentValid()
    {
        auto& lib = Globals::Game().GetAnimationLibrary();
        return lib.find(currentAnimation) != lib.end();
    }

    bool RenderComponent::IsDefaultValid()
    {
        auto& lib = Globals::Game().GetAnimationLibrary();
        return lib.find(defaultAnimation) != lib.end();
    }

    const Animation& RenderComponent::GetCurrentAnimation()
    {
        return Globals::Game().GetAnimationLibrary().at(currentAnimation);
    }

    const Animation& RenderComponent::GetDefaultAnimation()
    {
        return Globals::Game().GetAnimationLibrary().at(defaultAnimation);
    }

    void RenderComponent::ParseActions(int startFrame, int endFrame)
    { // Usually the interval is one frame in size
        if (!IsCurrentValid())
        {
            return;
        }
        for (auto& elem : GetCurrentAnimation().GetActions())
        {
            std::string lastID = currentAnimation;
            switch (elem.criteria.what)
            {
                case Animation::AnimationCriteria::TriggerAtFrameX:
                {
                    int frame;
                    try
                    {
                        frame = std::stoi(elem.criteria.param);
                    } catch (...)
                    {
                        return; // Param is not parsable
                    }
                    if (Utility::InRange(frame, startFrame, endFrame))
                    {
                        ActivateAction(elem);
                    }
                }
                break;

                case Animation::AnimationCriteria::TriggerAtStart:
                {
                    if (!parsedStart && currentFrame == 0)
                    {
                        ActivateAction(elem);
                        parsedStart = true;
                    }
                }
                break;

                case Animation::AnimationCriteria::TriggerAtEnd:
                {
                    if (!parsedEnd && finished)
                    {
                        ActivateAction(elem);
                        parsedEnd = true;
                    }
                }
                break;
            }
            if (currentAnimation != lastID)
            {
                parsedStart = false;
                parsedEnd   = false;
                finished    = false;
                break;
            }
        }
    }

    void RenderComponent::ActivateAction(Animation::Action action)
    {
        switch (action.instruction.what)
        {
            case Animation::AnimationInstruction::ExecuteCallback:
            {
                GetCurrentAnimation().ExecCallback();
            }
            break;
            case Animation::AnimationInstruction::PlaySound:
            {
                Globals::Audio().PlaySound(action.instruction.param);
            }
            break;
            case Animation::AnimationInstruction::SwitchAnimation:
            {
                SwitchAnimation(action.instruction.param);
            }
            break;
        }
    }

    void RenderComponent::SetDefaultAnimation(const std::string& ID)
    {
        auto& lib = Globals::Game().GetAnimationLibrary();
        if (lib.find(ID) == lib.end() ||
            std::find(animations.begin(), animations.end(), ID) == animations.end())
        {
            return;
        }
        defaultAnimation = ID;
    }

    void RenderComponent::AddAnimation(const std::string& ID)
    {
        auto& lib = Globals::Game().GetAnimationLibrary();
        if (lib.find(ID) == lib.end() ||
            std::find(animations.begin(), animations.end(), ID) != animations.end())
        {
            return;
        }
        animations.push_back(ID);
    }

    void RenderComponent::SwitchAnimation(const std::string& ID)
    {
        if (std::find(animations.begin(), animations.end(), ID) == animations.end())
        {
            return;
        }
        parsedStart      = false;
        parsedEnd        = false;
        currentAnimation = ID;

        auto& info = GetCurrentAnimation().GetInfo();

        updatesPerFrame  = info.updatesPerFrame;
        framesPerRow     = info.framesPerRow;
        framesPerCollumn = info.framesPerCollumn;
        totalFrames      = info.framesPerCollumn * info.framesPerRow;
        mode             = info.mode;

        SetTexture(GetCurrentAnimation().GetAnimationTexture());
        SetCenter(GetCurrentAnimation().GetCustomCenter());

        Restart();
    }

    void RenderComponent::SwitchToDefault()
    {
        if (IsDefaultValid())
        {
            parsedStart      = false;
            parsedEnd        = false;
            currentAnimation = defaultAnimation;

            auto& info = GetCurrentAnimation().GetInfo();

            updatesPerFrame  = info.updatesPerFrame;
            framesPerRow     = info.framesPerRow;
            framesPerCollumn = info.framesPerCollumn;
            totalFrames      = info.framesPerCollumn * info.framesPerRow;
            mode             = info.mode;

            SetTexture(GetCurrentAnimation().GetAnimationTexture());
            SetCenter(GetCurrentAnimation().GetCustomCenter());

            Restart();
        }
    }

    const std::string& RenderComponent::GetCurrentAnimationID()
    {
        return currentAnimation;
    }

    void RenderComponent::Update(int frameUpdates)
    {
        int lastFrame = currentFrame;
        AnimatedSprite::Update(frameUpdates);
        if (currentFrame > lastFrame)
        {
            ParseActions(lastFrame + 1, currentFrame);
        }
        else if (finished)
        {
            std::string lastID = currentAnimation;
            if (!parsedEnd)
            {
                ParseActions(totalFrames, totalFrames);
            }
            if (currentAnimation == lastID)
            {
                SwitchToDefault();
            }
            AnimatedSprite::Update(1);
        }
    }

} // namespace Game