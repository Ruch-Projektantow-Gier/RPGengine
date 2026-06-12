#pragma once

#include <stdexcept>

#include <webgpu/webgpu_cpp.h>
#include <glm/glm.hpp>

#include "Buffer.hpp"

namespace rpg::ren::wgp {
    inline constexpr size_t MaxUniforms = 200;

    struct Uniform {
        glm::mat4 M;
        glm::vec4 color;
    };

    struct StackUniformBuffer {
        wgpu::Buffer _buffer;
        size_t _offset;
        uint32_t _stride;

        using VoidPointer = buffer::VoidPointer;
        template <typename T> using Pointer = buffer::Pointer<T>;
        using ThinVoidPointer = buffer::ThinVoidPointer;
        template <typename T> using ThinPointer = buffer::ThinPointer<T>;

        template <typename T = Uniform>
        struct Uniforms {
            const wgpu::Queue& queue;
            const wgpu::Buffer& buffer;
            size_t offset;

            void write(const T& data) const {
                queue.WriteBuffer(
                    buffer,
                    offset,
                    &data,
                    sizeof(T)
                );
            }
            void operator=(const T& data) const {
                write(data);
            }
        };

        struct pointer {
            const wgpu::Buffer& buffer;
            size_t offset;
            size_t size;
            void write(const wgpu::Queue& queue, const void* data) {
                queue.WriteBuffer(buffer, offset, &data, size);
            }
        };

    private:
        StackUniformBuffer(
            wgpu::Buffer&& buffer,
            uint32_t stride
        ) : _buffer(buffer), _offset(0), _stride(stride) {}

        inline static constexpr size_t ceilToNextMultiple(size_t value, size_t alignment) {
            size_t multiple = value / alignment + (value % alignment == 0 ? 0 : 1);
            return multiple * alignment;
        };

        size_t ceilToNextMultiple(size_t value) {
            return ceilToNextMultiple(value, _stride);
        };

    public:
        StackUniformBuffer(const wgpu::Device& device) : StackUniformBuffer([&device](){
            wgpu::Limits deviceLimits;
            if(!device.GetLimits(&deviceLimits))
                throw std::runtime_error("Could not get device limits\n");
            uint32_t alignment = deviceLimits.minUniformBufferOffsetAlignment;
            size_t stride = ceilToNextMultiple(sizeof(Uniform), alignment);
            wgpu::BufferDescriptor desc {
                .label = "Uniform Buffer",
                .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
                .size = stride * MaxUniforms,
                .mappedAtCreation = false,
            };
            return StackUniformBuffer(
                device.CreateBuffer(&desc),
                stride
            );
        }()) {}

        VoidPointer allocate(size_t size) {
            size_t offset = _offset;
            _offset += ceilToNextMultiple(size);
            return VoidPointer(_buffer, offset, size);
        }

        void deallocate(const VoidPointer& ptr) {
            size_t stride = ceilToNextMultiple(ptr._size);
            if (_offset - stride == ptr._offset) {
                _offset = ptr._offset;
            }
        }

        template <typename T>
        Pointer<T> allocate() {
            size_t offset = _offset;
            _offset += ceilToNextMultiple(sizeof(T));
            return VoidPointer(_buffer, offset, sizeof(T));
        }

        template <typename T>
        void deallocate(const Pointer<T>& ptr) {
            size_t stride = ceilToNextMultiple(sizeof(T));
            if (_offset - stride == ptr._offset) {
                _offset = ptr._offset;
            }
        }

        template <typename T = Uniform>
        Uniforms<T> push(const wgpu::Queue& queue) {
            Uniforms<T> uniforms {
                .queue = queue,
                .buffer = _buffer,
                .offset = _offset
            };
            _offset += ceilToNextMultiple(sizeof(T));
            return uniforms;
        }

        template <typename T = Uniform>
        Uniforms<T> push(const wgpu::Queue& queue, const T& data) {
            auto uniforms = push<T>(queue);
            uniforms = data;
            return uniforms;
        }

        template <typename T = Uniform>
        pointer push() {
            pointer ptr {
                .buffer = _buffer,
                .offset = _offset,
                .size = sizeof(T)
            };
            _offset += ceilToNextMultiple(ptr.size);
            return ptr;
        }
        template<typename T> void pop() {
            _offset -= ceilToNextMultiple(sizeof(T));
        }
    };
}