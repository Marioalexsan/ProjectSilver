#ifndef SHADOWHP_HEADER
#define SHADOWHP_HEADER

#include "PCHeader.h"
#include "Sprite.h"

namespace Game {
	class ShadowHPBar : public Sprite {
	protected:
		SDL_Texture* lastStandHPtex;
		SDL_Texture* HPtex;

		double HPValue;
		double lastStandHPValue;
		double maxHP;
	public:
		ShadowHPBar();

		void SetHPTexture(const string& texture);
		void SetLastStandHPTexture(const string& texture);

		void SetHPValues(double HP, double lastStandHP, double maxHP);

		virtual void Draw();
	};
}

#endif