#pragma once
#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    namespace shadermodule {
        inline wgpu::ShaderModule create(
            const wgpu::Device& device, std::string_view code
        ){
            wgpu::ShaderSourceWGSL wgsl { { .nextInChain = nullptr, .code = code } };
            wgpu::ShaderModuleDescriptor shaderModuleDescriptor { .nextInChain = &wgsl };
            return device.CreateShaderModule(&shaderModuleDescriptor);
        }
    }
    struct ShaderModule {
        inline static wgpu::ShaderModule create(
            const wgpu::Device& device, std::string_view code
        ) {
            return shadermodule::create(device, code);
        }
    };
}