#pragma once
#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    namespace bindgroup {
        namespace layout {
            inline wgpu::BindGroupLayout create(
                const wgpu::Device &device,
                const wgpu::BindGroupLayoutDescriptor& descriptor
            ) {
                return device.CreateBindGroupLayout(&descriptor);
            }
            inline wgpu::BindGroupLayout create(
                const wgpu::Device& device,
                const wgpu::BindGroupLayoutEntry& entry,
                std::string_view label = std::string_view()
            ) {
                return create(device, wgpu::BindGroupLayoutDescriptor{
                    .label = label,
                    .entryCount = 1,
                    .entries = &entry
                });
            }
            inline wgpu::BindGroupLayout create(
                const wgpu::Device &device,
                std::span<const wgpu::BindGroupLayoutEntry> entries,
                std::string_view label = std::string_view()
            ) {
                return create(device, wgpu::BindGroupLayoutDescriptor{
                    .label = label,
                    .entryCount = entries.size(),
                    .entries = entries.data()
                });
            }
            inline wgpu::BindGroupLayout create(
                const wgpu::Device& device,
                std::initializer_list<const wgpu::BindGroupLayoutEntry> entries,
                std::string_view label = std::string_view()
            ) {
                return create(device, wgpu::BindGroupLayoutDescriptor{
                    .label = label,
                    .entryCount = entries.size(),
                    .entries = entries.begin()
                });
            }

            struct BasicEntry {
                wgpu::ShaderStage visibility;
            };
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
                std::derived_from<BasicEntry> auto&&... entries
            ) {
                uint32_t binding = 0;
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
                return create(device, entrs, label);
            }
        };
        struct Layout {
            using BasicEntry = layout::BasicEntry;
            using BufferEntry = layout::BufferEntry;
            using TextureEntry = layout::TextureEntry;
            using SamplerEntry = layout::SamplerEntry;
            inline static wgpu::BindGroupLayout make(
                const wgpu::Device& device,
                std::string_view label,
                std::derived_from<layout::BasicEntry> auto&&... entries
            ) {
                return layout::make(device, label, std::forward<decltype(entries)>(entries)...);
            }
        };
    };
    struct BindGroup {
        using Layout = bindgroup::Layout;
    };
}