#include <thread>
#include <RPGengine/RPGengine.hpp>

RPGENGINE_API void run(rpg::Game* game){
	rpg::Game* Game = static_cast<rpg::Game*>(game);
	for (size_t i = 0; i < 3; ++i) {
		Game->updateECS(1.0f);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}