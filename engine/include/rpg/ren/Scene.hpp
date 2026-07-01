#pragma once
#include <cstdint>
#include <vector>

#include <rpg/math/Vec3.hpp>

namespace rpg::ren {
    struct Shader { };
    namespace shaders {
        struct Lit : Shader {
            struct Material {
                uint32_t colorTextureId;
            };
            std::vector<Material> materials;
        };
    }
    struct Scene {
        struct Entry {
            uint32_t materialId;
            uint32_t meshId;
            math::Vec3f position;
            math::Vec3f rotation;
            math::Vec3f scale;
        };
        size_t materialsCount;
        std::vector<Entry> objects;
        struct {
            math::Vec3f position;
            math::Vec3f center = math::Vec3f(0.0f);
            float fov = 70.0f;
            float near = 0.1f;
            float far = 1000.0f;
        } camera;
    };
}