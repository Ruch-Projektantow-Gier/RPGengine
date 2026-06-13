#pragma once

#include <rpg/ren/Scene.hpp>

namespace rpg {
    void runGame(ren::Scene&& Scene, void(*onUpdate)(ren::Scene&, float));
}