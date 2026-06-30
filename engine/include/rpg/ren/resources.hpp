#pragma once

#include "texture.hpp"

namespace rpg::ren::resources {
    struct CreateInfo {
        size_t maxObjects;
        texturearray::CreateInfo textureData;
    };
}