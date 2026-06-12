#pragma once
#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    namespace BindGroup {
        namespace Layout {
            struct BasicEntry {
                wgpu::ShaderStage visibility;
            };
            template <typename T>
            concept EntryLike = std::derived_from<T, BasicEntry>;

            struct BufferEntry : BasicEntry {
                wgpu::BufferBindingLayout buffer;
            };
            struct TextureEntry : BasicEntry {
                wgpu::TextureBindingLayout texture;
            };
            struct SamplerEntry : BasicEntry {
                wgpu::SamplerBindingLayout sampler;
            };
            wgpu::BindGroupLayout make(
                const wgpu::Device& device,
                std::string_view label,
                EntryLike auto&&... entries
            ) {
                size_t binding = 0;
                wgpu::BindGroupLayoutEntry entrs[sizeof...(entries)];
                ([&binding, &entrs](auto&& entr) {
                    using T = std::decay_t<decltype(entr)>;
                    entrs[binding].binding = binding;
                    entrs[binding].visibility = entr.visibility;
                    if constexpr (std::is_same_v<T, BufferEntry>)
                        entrs[binding].buffer = entr.buffer;
                    if constexpr (std::is_same_v<T, TextureEntry>)
                        entrs[binding].texture = entr.texture;
                    if constexpr (std::is_same_v<T, SamplerEntry>)
                        entrs[binding].sampler = entr.sampler;
                    binding += 1;
                }(entries), ...);
                wgpu::BindGroupLayoutDescriptor desc {
                    .label = label,
                    .entryCount = sizeof...(entries),
                    .entries = entrs
                };
                return device.CreateBindGroupLayout(&desc);
            }
        };
    };
}