#pragma once

#include "webgpu/webgpu_cpp.h"

#include "Buffer.hpp"

namespace rpg::ren::wgp {
    struct StackPoolAllocator {
        struct AnyPointer {
            const wgpu::Buffer& buffer;
            size_t offset;
            size_t size;

            void write(const wgpu::Queue& queue, const void* data) {
                queue.WriteBuffer(buffer, offset, &data, size);
            }
            template <typename T>
            void write(const wgpu::Queue& queue, const T& data) {
                assert(sizeof(T) == size);
                write(queue, &data);
            }
        };

    private:
        const wgpu::Buffer& _buffer;
        size_t _offset;

    public:
        StackPoolAllocator(
            const wgpu::Buffer& buffer,
            size_t offset = 0
        ) : _buffer(buffer), _offset(offset) {}

        template <typename T>
        Buffer::Pointer<T> allocate(size_t count = 1) {
            Buffer::Pointer<T> ptr {
                .buffer = _buffer,
                .offset = _offset
            };
            _offset += count * sizeof(T);
            return ptr;
        }

        template <typename T>
        void deallocate(const Buffer::Pointer<T>& ptr, size_t count = 1) {
            _offset -= count * sizeof(T);
            assert(ptr.offset == _offset);
        }
    };
}