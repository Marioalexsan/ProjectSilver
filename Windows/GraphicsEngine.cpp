#include "PCHeader.h"

#include "GraphicsEngine.h"

namespace Game {
	GraphicsEngine*		GraphicsEngine::currentEngine = nullptr;
	SDL_Window*			GraphicsEngine::Window = nullptr;
	SDL_Renderer*		GraphicsEngine::Renderer = nullptr;

	int	GraphicsEngine::windowWidth = 0;
	int	GraphicsEngine::windowHeight = 0;

	double	GraphicsEngine::centeredCameraX = 0;
	double	GraphicsEngine::centeredCameraY = 0;

	const map<string, GraphicsEngine::VideoMode> GraphicsEngine::VideoModes = {
		{"1920.1080.f", {1920, 1080, true}},
		{"1600.900.f", {1600, 900, true}},
		{"1366.768.f", {1366, 768, true}},
		{"1280.720.f", {1280, 720, true}},
		{"1920.1080.w", {1920, 1080, false}},
		{"1600.900.w", {1600, 900, false}},
		{"1366.768.w", {1366, 768, false}},
		{"1280.720.w", {1280, 720, false}}
	};

	GraphicsEngine::GraphicsEngine() :
		currentID(0)
	{
		windowWidth = 1920;
		windowHeight = 1080;
		renderWidth = 1920;
		renderHeight = 1080;
		if (currentEngine == nullptr) {
			currentEngine = this;
		}
		//SDL_RenderSetClipRect(Renderer, &Utility::MakeSDLRect(0, 0, ResolutionTargetWidth, ResolutionTargetHeight));
		SetDisplayMode(VideoModes.at("1920.1080.w"));
	}

	GraphicsEngine::~GraphicsEngine() {
		if (currentEngine == this) {
			currentEngine = nullptr;
		}
	}

	pair<int, int> GraphicsEngine::GetWindowSize() {
		return { windowWidth, windowHeight };
	}
	
	bool GraphicsEngine::SetDisplayMode(VideoMode mode) {
		renderWidth = mode.width;
		renderHeight = mode.height;
		if (mode.fullscreen) {
			SDL_SetWindowFullscreen(GraphicsEngine::Window, SDL_WINDOW_FULLSCREEN);
			SDL_DisplayMode display;
			display.w = mode.width;
			display.h = mode.height;
			display.driverdata = 0;
			display.refresh_rate = 0;
			display.format = SDL_PIXELFORMAT_RGBA32;
			SDL_SetWindowDisplayMode(Window, &display);
			SDL_GetWindowSize(Window, &windowWidth, &windowHeight);
		}
		else {
			SDL_SetWindowFullscreen(GraphicsEngine::Window, 0);
			SDL_SetWindowSize(Window, mode.width, mode.height);
			windowWidth = mode.width;
			windowHeight = mode.height;
		}
		if (!mode.fullscreen) {
			SDL_RenderSetScale(Renderer, float(renderWidth) / ResolutionTargetWidth, float(renderHeight) / ResolutionTargetHeight);
		}
		else {
			SDL_RenderSetScale(Renderer, 1.0f, 1.0f);
		}
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

	int GraphicsEngine::AddDrawable(Drawable* element) {
		int ID = NextID();
		drawableLibrary[ID] = element;
		return ID;
	}

	bool GraphicsEngine::RemoveDrawable(int ID) {
		if (drawableLibrary.find(ID) == drawableLibrary.end()) {
			return false;
		}
		drawableLibrary.erase(ID);
		return true;
	}

	void GraphicsEngine::ClearDrawables() {
		drawableLibrary.empty();
	}

	void GraphicsEngine::RenderAll() {
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);
		for (auto& elem : drawableLibrary) {
			elem.second->Draw();
		}
		SDL_RenderPresent(Renderer);
	}

	void GraphicsEngine::SetCamera(double x, double y)
	{
		centeredCameraX = x;
		centeredCameraY = y;
	}

	pair<double, double> GraphicsEngine::GetCamera() {
		return { centeredCameraX, centeredCameraY };
	}

	void GraphicsEngine::PushCamera(double x, double y)
	{
		centeredCameraX += x;
		centeredCameraY += y;
	}

	// Has Camera feature
	// Has Offset feature
	void GraphicsEngine::RenderCopyExWithCamera(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, double angle, const SDL_Point* center, SDL_RendererFlip flip) {
		
		if (dstrect) {
			SDL_Rect newdstrect = Utility::MakeSDLRect(dstrect->x - centeredCameraX - center->x, dstrect->y - centeredCameraY - center->y, dstrect->w, dstrect->h);
			SDL_RenderCopyEx(Renderer, texture, srcrect, &newdstrect, angle, center, flip);
		}
		else {
			SDL_RenderCopyEx(Renderer, texture, srcrect, dstrect, angle, center, flip);
		}
		
	}

}