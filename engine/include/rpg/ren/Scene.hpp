#pragma once
#include <cstdint>
#include <vector>

#include <rpg/math/Vec.hpp>

namespace rpg::ren {
    struct Scene {
        struct Entry {
            uint32_t materialId;
            uint32_t meshId;
            math::Vec3<float> position;
            math::Vec3<float> rotation;
            math::Vec3<float> scale;
        };
        std::vector<Entry> entries;
    };
}