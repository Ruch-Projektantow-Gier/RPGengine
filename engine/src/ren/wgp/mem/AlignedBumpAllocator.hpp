#pragma once

#include <webgpu/webgpu_cpp.h>

#include "../Buffer.hpp"
#include "../getLimits.hpp"

namespace rpg::ren::wgp::mem {
    inline constexpr size_t ceilToNextMultiple(size_t value, size_t alignment) {
        size_t multiple = value / alignment + (value % alignment == 0 ? 0 : 1);
        return multiple * alignment;
    };

    struct AlignedBumpAllocatedBuffer {
    private:
        wgpu::Buffer _buffer;
        size_t _offset;
        size_t _alignment;

        size_t ceilToNextMultiple(size_t value) {
            return mem::ceilToNextMultiple(value, _alignment);
        };

    public:
        AlignedBumpAllocatedBuffer(
            wgpu::Buffer&& buffer,
            size_t alignment
        ) : _buffer(std::move(buffer)), _offset(0), _alignment(alignment) {}

        const wgpu::Buffer& buffer() const { return _buffer; }
        wgpu::Buffer buffer() { return _buffer; }
        size_t alignment() { return _alignment; }

        void free() { _offset = 0; }
        void deallocateAssumeAlignment(size_t size) {
            assert(size % _alignment == 0);
            assert(_offset >= size);
            _offset -= size;
        }
        void deallocate(size_t size) {
            deallocateAssumeAlignment(ceilToNextMultiple(size));
        }
        template<typename T> void deallocate() { deallocate(sizeof(T)); }

        buffer::VoidPointer allocate(size_t size) {
            size_t offset = _offset;
            _offset += ceilToNextMultiple(size);
            return buffer::VoidPointer(_buffer, offset, size);
        }

        void deallocate(const buffer::VoidPointer& ptr) {
            size_t size = ceilToNextMultiple(ptr._size);
            assert(_offset == ptr._offset + size);
            _offset -= size;
        }

        template <typename T>
        buffer::Pointer<T> allocate() {
            size_t offset = _offset;
            _offset += ceilToNextMultiple(sizeof(T));
            return buffer::Pointer<T>(_buffer, offset);
        }

        template <typename T>
        buffer::PointerWithQueue<T> allocate(const wgpu::Queue& queue) {
            size_t offset = _offset;
            _offset += ceilToNextMultiple(sizeof(T));
            return buffer::PointerWithQueue<T>(queue, _buffer, offset);
        }

        template <typename T>
        buffer::PointerWithQueue<T> allocate(const wgpu::Queue& queue, const T& data) {
            buffer::PointerWithQueue<T> ptr = allocate<T>(queue);
            ptr.write(data);
            return ptr;
        }
    };

    template <typename T>
    struct AlignedTypedBumpAllocatedBuffer {
    private:
        wgpu::Buffer _buffer;
        size_t _offset;
        size_t _stride;

    public:
        AlignedTypedBumpAllocatedBuffer(
            wgpu::Buffer&& buffer,
            size_t alignment
        ) : _buffer(std::move(buffer)),
            _offset(0),
            _stride(ceilToNextMultiple(sizeof(T), alignment))
        {}

        const wgpu::Buffer& buffer() const { return _buffer; }
        wgpu::Buffer buffer() { return _buffer; }
        size_t stride() { return _stride; }

        void free() { _offset = 0; }
        void deallocate(size_t count) {
            assert(_offset > count * _stride);
            _offset -= count * _stride;
        }

        buffer::Pointer<T> allocate() {
            size_t offset = _offset;
            _offset += _stride;
            return buffer::Pointer<T>(_buffer, offset);
        }

        void deallocate(const buffer::Pointer<T>& ptr) {
            assert(_offset == ptr.offset() + _stride);
            _offset -= _stride;
        }

        buffer::PointerWithQueue<T> allocate(const wgpu::Queue& queue) {
            size_t offset = _offset;
            _offset += _stride;
            return buffer::PointerWithQueue<T>(queue, _buffer, offset);
        }

        buffer::PointerWithQueue<T> allocate(const wgpu::Queue& queue, const T& data) {
            buffer::PointerWithQueue<T> ptr = allocate(queue);
            ptr.write(data);
            return ptr;
        }
    };

    inline AlignedBumpAllocatedBuffer bumpAllocatedUniformBuffer(
        const wgpu::Device& device, size_t size
    ) {
        return AlignedBumpAllocatedBuffer(buffer::create(device, {
            .label = "Uniform Buffer",
            .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
            .size = size,
            .mappedAtCreation = false
        }), getLimits(device).minUniformBufferOffsetAlignment);
    }
}