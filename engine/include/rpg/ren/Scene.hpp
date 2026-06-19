#pragma once
#include <cstdint>
#include <vector>

#include <rpg/math/Vec3.hpp>

namespace rpg::ren {
    struct Scene {
        struct Entry {
            uint32_t materialId;
            uint32_t meshId;
            math::Vec3f position;
            math::Vec3f rotation;
            math::Vec3f scale;
        };
        std::vector<Entry> entries;
    };
}