#ifndef GRAPHICS_HEADER
#define GRAPHICS_HEADER

#include <SDL.h>

#include "AssetManager.h"
#include "STLHeader.h"
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

		void SetCamera(int x, int y, int w, int h);
	};
}

#endif