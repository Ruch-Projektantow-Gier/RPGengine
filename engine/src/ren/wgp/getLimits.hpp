#pragma once

#include <stdexcept>

#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    inline wgpu::Limits getLimits(const wgpu::Device& device) {
        wgpu::Limits limits;
        if (!device.GetLimits(&limits))
            throw std::runtime_error("Could not get WebGPU device limits");
        return limits;
    }
}