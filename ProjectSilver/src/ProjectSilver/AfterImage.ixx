module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.AfterImage;
import ProjectSilver.Effect;

export namespace Game
{
    class AfterImage : public Effect
    {
    protected:
        sf::Texture* image;
        int          targetFrame;

        int framesPerRow;
        int framesPerCollumn;

        int targetFadeCounter;
        int fadeCounter;

    public:
        AfterImage();
        ~AfterImage();

        void SetStaticTexture(const std::string& texture);
        void SetAnimationFrame(const std::string& animation, int frame = 0);

        inline void SetFadeTime(int fade)
        {
            fadeCounter = targetFadeCounter = fade;
        }

        virtual void Update();
        virtual void Draw();
    };
} // namespace Game
