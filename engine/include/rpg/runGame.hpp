#pragma once

#include <rpg/ren/Scene.hpp>
#include <rpg/ren/resources.hpp>

namespace rpg {
    void runGame(
        const ren::resources::CreateInfo& CreateInfo,
        const ren::Scene& Scene,
        void(*onUpdate)(ren::Scene&, float)
    );
    void runGame(
        const ren::resources::CreateInfo& CreateInfo,
        ren::Scene&& Scene,
        void(*onUpdate)(ren::Scene&, float)
    );
}