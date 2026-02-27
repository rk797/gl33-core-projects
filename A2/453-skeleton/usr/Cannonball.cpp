#include "Cannonball.h"
#include "LocalPlayer.h"


extern LocalPlayer* pLocalPlayer;


Cannonball::Cannonball()
	:GameObject(_Core.cannonTexture->getDimensions(), 0.02f, pLocalPlayer->position)
{
	creation_time = std::chrono::steady_clock::now();
	forward = pLocalPlayer->orthogDir;
}