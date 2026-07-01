#pragma once

#include "texture.hpp"
#include "Material.hpp"

namespace rpg::ren::resources {
    struct CreateInfo {
        size_t maxObjects;
        std::span<const ren::texture::DataSource> textureData;
        std::span<const ren::Material> materials;
    };
}