#ifndef GLOBALS_HEADER
#define GLOBALS_HEADER

// Deoarece voi avea doar o instanta de joc la un moment dat
// Ar avea sens de salvat acea instanta
// Salvand-o, as putea reduce din complexitatea
// Functiilor care necesita acces la assets, graphics engine, etc.
// (nu mai trebuie sa salvez un pointer redundant laasset manager, etc)


// Cel putin, sper ca utilizarea variabilelor (efectiv) globale poate fi justificata aici

#include "PCHeader.h"
#include "GameMaster.h"

namespace Game {
	class Globals {
	private:
		static GameMaster* theGame;
	public:
		// Referenceul ma face mai confident ca obiectul este valid
		static void SetTheGame(GameMaster& game);

		static void GameDeaded();

		static GameMaster& Game();
		static AudioEngine& Audio();
		static GraphicsEngine& Graphics();
		static AssetManager& Assets();

	};
}

#endif