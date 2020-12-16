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
		static const map<string, VideoMode> VideoModes;

		static GraphicsEngine* currentEngine;
		static const int GUILayer = 255;
		static const int WorldBaseLayer = 0;

		static SDL_Window* Window;
		static SDL_Renderer* Renderer;
		static const int ResolutionTargetWidth = 1920;
		static const int ResolutionTargetHeight = 1080;

		static void RenderCopyExWithCamera(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, double angle, const SDL_Point* center, SDL_RendererFlip flip);

	private:
		// Current State Variables
		uint64_t currentID;

		int windowWidth;
		int windowHeight;

		int renderWidth;
		int renderHeight;

		bool bFullscreen;

		// Drawables Library
		map<int, Drawable*> drawableLibrary;


		uint64_t NextID();

		static double centeredCameraX;
		static double centeredCameraY;

	public:
		bool SetDisplayMode(VideoMode mode);
		void SetStandardViewport();
		void SetStandardScaling();

		GraphicsEngine();
		~GraphicsEngine();

		int AddDrawable(Drawable* element);
		bool RemoveDrawable(int ID);
		void ClearDrawables();

		void RenderAll();

		void SetCamera(double x, double y);
		pair<double, double> GetCamera();
		void PushCamera(double x, double y);
	};
}

#endif