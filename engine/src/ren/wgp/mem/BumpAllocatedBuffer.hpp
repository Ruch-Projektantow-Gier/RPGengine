#pragma once
#include <cstddef>

#include "webgpu/webgpu_cpp.h"

namespace rpg::ren::wgp {
    struct BumpAllocatedBuffer {
        wgpu::Buffer buffer;
        size_t offset;

        struct pointer {
            const wgpu::Buffer& buffer;
            size_t offset;
            size_t size;

            inline void write(const wgpu::Queue& queue, const void* data) {
                queue.WriteBuffer(buffer, offset, data, size);
            }

            inline void write(const wgpu::Queue& queue, const void* data, size_t offsetOffset, size_t overrideSize) {
                assert(offsetOffset + overrideSize <= size);
                queue.WriteBuffer(buffer, offset + offsetOffset, data, overrideSize);
            }
        };

        struct Allocation {
            const wgpu::Buffer& buffer;
            size_t offset;
            size_t size;
        };

        template <typename T>
        struct Pointer {
            const wgpu::Buffer& buffer;
            size_t offset;

            inline void write(const wgpu::Queue& queue, const T& data) {
                queue.WriteBuffer(buffer, offset, &data, sizeof(T));
            }
        };

        BumpAllocatedBuffer(
            const wgpu::Device& device,
            const wgpu::BufferDescriptor& desc
        ) : buffer(device.CreateBuffer(&desc)), offset(0) {}

        template <typename T>
        pointer push(size_t size) {
            pointer ptr = {
                .buffer = buffer,
                .offset = offset,
                .size = size * sizeof(T)
            };
            offset += ptr.size;
            return ptr;
        }
        pointer push(const auto& data, const wgpu::Queue& queue) {
            using Container = std::remove_const_t<std::remove_reference_t<decltype(data)>>;
            pointer ptr = push<typename Container::value_type>(data.size());
            ptr.write(queue, data.data());
            return ptr;
        }
        void pop(size_t size) { offset -= size; }
        template <typename T> void pop(size_t count) { pop(count * sizeof(T)); }
    };
}