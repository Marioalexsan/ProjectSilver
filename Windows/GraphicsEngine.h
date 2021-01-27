#ifndef GRAPHICS_HEADER
#define GRAPHICS_HEADER

#include "PCHeader.h"

#include "AssetManager.h"
#include "MiscUtility.h"
#include "Drawable.h"

namespace Game {
	class GraphicsEngine {
	public:
		struct VideoMode {
			int width;
			int height;
			bool fullscreen;
		};

		

		enum CommonLayers {
			WorldBase = 0,
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
		static const map<string, VideoMode> VideoModes;

		static const int ResolutionCount = 7;
		static const pair<int, int> Resolutions[ResolutionCount];

		static SDL_Window* Window;
		static SDL_Renderer* Renderer;

		static const int ResolutionTargetWidth = 1920;
		static const int ResolutionTargetHeight = 1080;

		static void RenderCopyExAdvanced(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, double angle, const SDL_Point* center, SDL_RendererFlip flip, bool useCamera = true);
	private:
		// Current State Variables
		uint64_t currentID;

		static int windowWidth;
		static int windowHeight;

		static int renderWidth;
		static int renderHeight;

		bool fullscreen;

		// Library of Drawable objects
		map<uint64_t, Drawable*> drawableLibrary;

		static double gameWinFadeout;

		uint64_t NextID();

		static Vector2 cameraPosition;

	public:
		GraphicsEngine();
		~GraphicsEngine();

		static Vector2 GetWindowSize();
		bool SetDisplayMode(VideoMode mode);
		inline VideoMode GetRenderDisplayMode() {
			return { renderWidth, renderHeight, fullscreen };
		}
		void SetStandardViewport();

		inline bool IsFullscreen() { return fullscreen; }
		inline void SetFullscreen(bool mode) { fullscreen = mode; }

		inline void SetGameWinFadeout(double value) { gameWinFadeout = Utility::ClampValue(value, 0.0, 100.0); }

		uint64_t AddDrawable(Drawable* element);
		bool	 RemoveDrawable(uint64_t ID);
		void	 ClearDrawables();

		void RenderAll();
		void EmptyRender();

		void	SetCameraPosition(Vector2 position);
		Vector2	GetCameraPosition();
		void	CenterCameraOn(Vector2 position);

		void	PushCamera(Vector2 amount);
	};
}

#endif