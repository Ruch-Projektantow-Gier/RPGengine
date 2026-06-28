#pragma once

#include <rpg/ren/Scene.hpp>

namespace rpg {
    struct TextureDataView {
        uint32_t width;
        uint32_t height;
        uint32_t count;
        const void* data;
    };
    void runGame(
        TextureDataView TextureData,
        const ren::Scene& Scene,
        void(*onUpdate)(ren::Scene&, float)
    );
    void runGame(
        TextureDataView TextureData,
        ren::Scene&& Scene,
        void(*onUpdate)(ren::Scene&, float)
    );
}