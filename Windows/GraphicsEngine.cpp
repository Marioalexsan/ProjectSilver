#include "PCHeader.h"

#include "GraphicsEngine.h"
#include "Globals.h"

namespace Game {
	SDL_Window*		GraphicsEngine::Window = nullptr;
	SDL_Renderer*	GraphicsEngine::Renderer = nullptr;

	int	GraphicsEngine::windowWidth = 0;
	int	GraphicsEngine::windowHeight = 0;
	int GraphicsEngine::renderWidth = 0;
	int GraphicsEngine::renderHeight = 0;

	double GraphicsEngine::gameWinFadeout = 0.0;

	Vector2	GraphicsEngine::cameraPosition = { 0.0, 0.0 };

	// Deprecated
	const map<string, GraphicsEngine::VideoMode> GraphicsEngine::VideoModes = {
		{"1920.1080.f", {1920, 1080, true}},
		{"1920.1080.w", {1920, 1080, false}},
		{"1600.900.f", {1600, 900, true}},
		{"1600.900.w", {1600, 900, false}},
		{"1366.768.f", {1366, 768, true}},
		{"1366.768.w", {1366, 768, false}},
		{"1280.720.f", {1280, 720, true}},
		{"1280.720.w", {1280, 720, false}}
	};

	const pair<int, int> GraphicsEngine::Resolutions[GraphicsEngine::ResolutionCount] = {
		{2560, 1440},
		{1920, 1080},
		{1680, 1050},
		{1600, 900},
		{1440, 900},
		{1366, 768},
		{1280, 720}
		//{1024, 768}
		//{800, 600}
	};

	GraphicsEngine::GraphicsEngine() :
		currentID(1)
	{
		windowWidth = 1280;
		windowHeight = 1280;
		renderWidth = 1280;
		renderHeight = 1280;
		SetDisplayMode({ 1280, 720, false });
	}

	GraphicsEngine::~GraphicsEngine() {
	}

	Vector2 GraphicsEngine::GetWindowSize() {
		return { windowWidth, windowHeight };
	}
	
	bool GraphicsEngine::SetDisplayMode(VideoMode mode) {
		renderWidth = mode.width;
		renderHeight = mode.height;
		SDL_RenderSetLogicalSize(GraphicsEngine::Renderer, ResolutionTargetWidth, ResolutionTargetHeight);
		if (mode.fullscreen) {
			SDL_DisplayMode display = { SDL_PIXELFORMAT_RGBA32, mode.width, mode.height, 60, 0 };
			if (fullscreen == mode.fullscreen) {
				// EmptyRender and position should hopefully reduce epileptic crap from resolution change
				EmptyRender();
				SDL_SetWindowFullscreen(GraphicsEngine::Window, 0);
				SDL_SetWindowPosition(GraphicsEngine::Window, 0, 0);
			}
			if (SDL_SetWindowDisplayMode(Window, &display) == -1) {
				SDL_SetWindowDisplayMode(Window, nullptr);
			}
			SDL_SetWindowFullscreen(GraphicsEngine::Window, SDL_WINDOW_FULLSCREEN);
			SDL_GetWindowDisplayMode(Window, &display);
			windowWidth = display.w;
			windowHeight = display.h;
		}
		else {
			if (fullscreen != mode.fullscreen) {
				SDL_SetWindowFullscreen(GraphicsEngine::Window, 0);
			}
			SDL_SetWindowSize(Window, mode.width, mode.height);
			SDL_GetWindowSize(Window, &windowWidth, &windowHeight);

			// Position it so that user can move it around afterwards (or just see most of the window)
			SDL_SetWindowPosition(GraphicsEngine::Window, 0, 30);
		}
		fullscreen = mode.fullscreen;
		SDL_RenderSetClipRect(Renderer, &Utility::MakeSDLRect(0, 0, ResolutionTargetWidth, ResolutionTargetWidth));
		return true;
	}

	void GraphicsEngine::SetStandardViewport() {
		SDL_RenderSetViewport(Renderer, &Utility::MakeSDLRect(0, 0, windowWidth, windowHeight));
	}

	uint64_t GraphicsEngine::NextID() {
		uint64_t val = currentID++;
		currentID = currentID == 0 ? 1 : currentID;
		return val;
	}

	uint64_t GraphicsEngine::AddDrawable(Drawable* element) {
		uint64_t ID = NextID();
		drawableLibrary[ID] = element;
		return ID;
	}

	bool GraphicsEngine::RemoveDrawable(uint64_t ID) {
		if (drawableLibrary.find(ID) == drawableLibrary.end()) {
			return false;
		}
		drawableLibrary.erase(ID);
		return true;
	}

	void GraphicsEngine::ClearDrawables() {
		drawableLibrary.clear();
	}

	void GraphicsEngine::RenderAll() {
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);

		map<int, vector<Drawable*>> layeredDrawables;

		for (auto& elem : drawableLibrary) {
			layeredDrawables[elem.second->GetLayer()].push_back(elem.second);
		}
		for (auto& elem : layeredDrawables) {
			for (auto& draw : elem.second) {
				draw->Draw();
			}
		}

		SDL_RenderPresent(Renderer);
	}

	void GraphicsEngine::EmptyRender() {
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);

		SDL_RenderPresent(Renderer);
	}

	void GraphicsEngine::SetCameraPosition(Vector2 position)
	{
		cameraPosition = position;
	}

	Vector2 GraphicsEngine::GetCameraPosition() {
		return cameraPosition;
	}

	void GraphicsEngine::CenterCameraOn(Vector2 position) {
		cameraPosition = position - Vector2(960.0, 540.0);
	}

	void GraphicsEngine::PushCamera(Vector2 pushAmount)
	{
		cameraPosition += pushAmount;
	}

	// Has Camera feature
	
	void GraphicsEngine::RenderCopyExAdvanced(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, double angle, const SDL_Point* center, SDL_RendererFlip flip, bool useCamera) {
		
		// Game Win Fadeout
		Color color;
		int reverseFadeout = Utility::ClampValue(100 - int(gameWinFadeout), 0, 100);

		SDL_GetTextureColorMod(texture, &color.r, &color.g, &color.b);
		SDL_SetTextureColorMod(texture, color.r * reverseFadeout / 100, color.g * reverseFadeout / 100, color.b * reverseFadeout / 100);

		if (dstrect) {
			SDL_Rect newdstrect = Utility::MakeSDLRect(int(dstrect->x - center->x), int(dstrect->y - center->y), dstrect->w, dstrect->h);
			if (useCamera) {
				newdstrect.x -= int(cameraPosition.x);
				newdstrect.y -= int(cameraPosition.y);
			}
			
			SDL_RenderCopyEx(Renderer, texture, srcrect, &newdstrect, angle, center, flip);
		}
		else {
			SDL_RenderCopyEx(Renderer, texture, srcrect, dstrect, angle, center, flip);
		}
		
	}
}