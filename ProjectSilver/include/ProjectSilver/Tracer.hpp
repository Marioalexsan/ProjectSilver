#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Effect.hpp>

namespace Game {

	// This class overrides Center, Position and Direction from Drawable by code nature
	class Tracer : public Effect {
		Vector2 start;
		Vector2 end;

		int targetFadeCounter;
		int fadeCounter;

		// Arbitratrily named values that make the tracer move slightly while fading
		Vector2 forwardSpeed;
		Vector2 sideSpeed;

		// Represents the body of a 90 degree tracer
		// Should be a Nx1 texture
		// Will be stretched across the whole body via SDL
		sf::Texture* body;

		// Represents the start cap of a 90 degree tracer, facing south with the end
		// End cap is rotated by 180 degrees relative to the start cap
		// May be null, which will cause no caps to be generated
		sf::Texture* endCap;

	public:
		Tracer(Vector2 start, Vector2 end);
		~Tracer();

		void SetBodyTexture(const std::string& texture);
		void SetCapTexture(const std::string& texture);

		inline void SetFadeTime(int fade) {
			fadeCounter = targetFadeCounter = fade;
		}

		virtual void Update();
		virtual void Draw();
	};
}
