module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.GraphicsEngine;
import ProjectSilver.AssetManager;
import ProjectSilver.Drawable;
import ProjectSilver.MiscUtility;

export namespace Game
{
    class GraphicsEngine
    {
    public:
        struct VideoMode
        {
            int  width;
            int  height;
            bool fullscreen;
        };


        enum CommonLayers
        {
            WorldBase  = 0,
            Background = 64,
            Floor,
            OnFloor,
            BelowDefaultHeight,
            DefaultHeight,
            BelowCeiling,
            Ceiling,
            BelowGUI = 191,
            GUI,
            AboveGUI,
            WorldTop = 255
        };

        // Deprecated
        static const std::map<std::string, VideoMode> VideoModes;

        static std::unique_ptr<sf::RenderWindow> Window;

        static const int ResolutionTargetWidth  = 1920;
        static const int ResolutionTargetHeight = 1080;

    private:
        // Current State Variables
        uint64_t currentID;

        static int windowWidth;
        static int windowHeight;

        static int renderWidth;
        static int renderHeight;

        bool fullscreen;

        // Library of Drawable objects
        std::map<uint64_t, Drawable*> drawableLibrary;

        static double gameWinFadeout;

        uint64_t NextID();

        static Vector2 cameraPosition;

    public:
        GraphicsEngine();
        ~GraphicsEngine();

        static Vector2   GetWindowSize();
        bool             SetDisplayMode(VideoMode mode);
        inline VideoMode GetRenderDisplayMode()
        {
            return {renderWidth, renderHeight, fullscreen};
        }
        void SetStandardViewport();

        inline bool IsFullscreen()
        {
            return fullscreen;
        }

        inline void SetGameWinFadeout(double value)
        {
            gameWinFadeout = Utility::ClampValue(value, 0.0, 100.0);
        }

        uint64_t AddDrawable(Drawable* element);
        bool     RemoveDrawable(uint64_t ID);
        void     ClearDrawables();

        void RenderAll();
        void EmptyRender();

        void    SetCameraPosition(Vector2 position);
        Vector2 GetCameraPosition();
        void    CenterCameraOn(Vector2 position);

        void PushCamera(Vector2 amount);
        void UseCamera(bool useCamera);
    };
} // namespace Game
