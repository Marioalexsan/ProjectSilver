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
			Floor = 65,
			OnFloor = 66,
			DefaultHeight = 67,
			BelowCeiling = 68,
			Ceiling = 69,
			GUI = 192,
			WorldTop = 255,
		};

		static const map<string, VideoMode> VideoModes;

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

		uint64_t NextID();

		static Vector2 cameraPosition;

	public:
		GraphicsEngine();
		~GraphicsEngine();

		static Vector2 GetWindowSize();
		bool SetDisplayMode(VideoMode mode);
		void SetStandardViewport();

		uint64_t AddDrawable(Drawable* element);
		bool	 RemoveDrawable(uint64_t ID);
		void	 ClearDrawables();

		void RenderAll();

		void	SetCameraPosition(Vector2 position);
		Vector2	GetCameraPosition();
		void	CenterCameraOn(Vector2 position);

		void	PushCamera(Vector2 amount);
	};
}

#endif