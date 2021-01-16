#include "PCHeader.h"
#include "Tracer.h"
#include "Globals.h"

namespace Game {
	Tracer::Tracer(Vector2 start, Vector2 end):
		fadeCounter(0),
		targetFadeCounter(0),
		start(start),
		end(end),
		body(nullptr),
		endCap(nullptr)
	{
		Vector2 directionVector = end - start;
		double speedPower = rand() % 40 / 100.0 + 0.6;
		double angle = Utility::ScrollValue(directionVector.Angle() - 10.0 + rand() % 20 / 1.0, 0.0, 360.0);
		startSpeed = Vector2::NormalVector(angle) * speedPower;
		speedPower = rand() % 40 / 100.0 + 0.6;
		angle = angle = Utility::ScrollValue(directionVector.Angle() - 10.0 + rand() % 20 / 1.0, 0.0, 360.0);
		endSpeed = Vector2::NormalVector(angle) * speedPower;
	}

	Tracer::~Tracer() {}

	void Tracer::SetBodyTexture(const string& texture) {
		auto lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(texture) == lib.end()) {
			//Error
			return;
		}
		this->body = lib.at(texture).texture;
	}

	void Tracer::SetCapTexture(const string& texture) {
		auto lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(texture) == lib.end()) {
			//Error
			return;
		}
		this->endCap = lib.at(texture).texture;
	}

	void Tracer::Update() {
		if (timeToLive <= 0) {
			return;
		}

		bool fading = fadeCounter <= targetFadeCounter / 2;
		double fadeFactor = double(fadeCounter)/ (targetFadeCounter / 2);

		if (fadeCounter > 0) {
			fadeCounter--;
			if (fading && targetFadeCounter > 0) {
				SetAlpha(GetAlpha() * fadeFactor);
			}
		}
		if (fading) {
			start += startSpeed * (2.5 - 2.0 * fadeFactor * fadeFactor);
			end += endSpeed * (2.5 - 2.0 * fadeFactor * fadeFactor);
		}
		else {
			start += startSpeed * 0.5;
			end += endSpeed * 0.5;
		}
		Effect::Update();
	}

	void Tracer::Draw() {
		using namespace Game::Utility;
		if (body == nullptr) {
			return;
		}

		// Body render
		// Cap render is UNIMPLEMENTED for now
		// AKA this class acts as an overglorified Sprite until that is done

		int w, h;
		SDL_QueryTexture(body, nullptr, nullptr, &w, &h);

		SDL_Rect src = MakeSDLRect(0, 0, w, h);
		SDL_Rect dest = MakeSDLRect(int(start.x), int(start.y), int((end - start).Length()), h);
		SDL_Point cen = MakeSDLPoint(0, h / 2);
		SDL_SetTextureAlphaMod(body, alpha);
		SDL_SetTextureColorMod(body, color.r, color.g, color.b);
		GraphicsEngine::RenderCopyExAdvanced(body, &src, &dest, (end - start).Angle() - 90.0, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);
	}
}