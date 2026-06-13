#pragma once
#include <string>
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>
#include <rpg/ren/wgp/StackBuffer.hpp>
#include <rpg/ren/texture.hpp>

namespace rpg::ren::wgp {
    struct TextRenderer {
        inline static constexpr size_t characterLimit = 100;
        struct CharData {
            glm::vec2 position;
            glm::vec2 size;
            glm::vec4 texcoord;
        };
        Texture fontTexture;
        wgpu::ShaderModule shaderModule;
        wgpu::BindGroupLayout bindGroupLayout;
        wgpu::PipelineLayout pipelineLayout;
        wgpu::RenderPipeline renderPipeline;
        wgpu::BindGroup bindGroup;
        StackBuffer::pointer instanceBufferPointer;
        size_t charCount;
        float aspectRatio;

        TextRenderer(
            const wgpu::Device& device,
            const wgpu::Sampler& sampler,
            wgpu::TextureFormat colorFormat,
            StackBuffer::pointer InstanceBufferPointer,
            float AspectRatio
        );
        void draw(
            const wgpu::Queue& queue,
            const std::string& text,
            glm::vec2 position,
            float scale
        );
        void draw(
            const wgpu::Device& device,
            const wgpu::Queue& queue,
            const wgpu::Surface& surface
        );
    };
}