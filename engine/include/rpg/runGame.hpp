#pragma once

#include <rpg/ren/Scene.hpp>

namespace rpg {
    void runGame(const ren::Scene& Scene, void(*onUpdate)(ren::Scene&, float));
    void runGame(ren::Scene&& Scene, void(*onUpdate)(ren::Scene&, float));
}