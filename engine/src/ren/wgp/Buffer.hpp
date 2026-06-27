#pragma once

#include "webgpu/webgpu_cpp.h"

namespace rpg::ren::wgp {
    namespace buffer {
        template <typename T> struct ThinPointer;
        struct ThinVoidPointer;

        template <typename T> struct Pointer;
        struct VoidPointer;

        template <typename T> struct PointerWithQueue;
        struct VoidPointerWithQueue;

        template <typename T>
        struct ThinPointer {
            size_t _offset;

            constexpr ThinPointer(size_t Offset) : _offset(Offset) {}

            explicit constexpr ThinPointer(ThinVoidPointer ptr);
            explicit constexpr ThinPointer(Pointer<T> ptr);
            explicit constexpr ThinPointer(VoidPointer ptr);

            constexpr size_t offset() const { return _offset; }
            constexpr size_t size() const { return sizeof(T); }

            void write(
                const wgpu::Queue& queue,
                const wgpu::Buffer& buffer,
                const T& data
            ) const {
                queue.WriteBuffer(
                    buffer,
                    _offset,
                    &data,
                    sizeof(T)
                );
            }
        };

        struct ThinVoidPointer {
            size_t _offset;
            size_t _size;

            constexpr ThinVoidPointer(size_t Offset, size_t Size) : _offset(Offset), _size(Size) {}


            template <typename T> explicit constexpr ThinVoidPointer(ThinPointer<T> ptr);
            template <typename T> explicit constexpr ThinVoidPointer(Pointer<T> ptr);
            explicit constexpr ThinVoidPointer(VoidPointer ptr);

            constexpr size_t offset() const { return _offset; }
            constexpr size_t size() const { return _size; }

            void write(
                const wgpu::Queue& queue,
                const wgpu::Buffer& buffer,
                const void* data
            ) const {
                queue.WriteBuffer(buffer, _offset, data, _size);
            }
        };

        template <typename T>
        struct Pointer {
            wgpu::Buffer _buffer;
            size_t _offset;

            constexpr Pointer(
                const wgpu::Buffer& Buffer, size_t Offset
            ) : _buffer(Buffer), _offset(Offset) {}
            constexpr Pointer(const VoidPointer& ptr);

            wgpu::Buffer buffer() { return _buffer; }
            const wgpu::Buffer& buffer() const { return _buffer; }
            constexpr size_t offset() const { return _offset; }
            constexpr size_t size() const { return sizeof(T); }

            void write(const wgpu::Queue& queue, const T& data) const {
                queue.WriteBuffer(_buffer, _offset, &data, sizeof(T));
            }
        };

        struct VoidPointer {
            wgpu::Buffer _buffer;
            size_t _offset;
            size_t _size;

            VoidPointer(
                const wgpu::Buffer& Buffer,
                size_t Offset, size_t Size
            ) : _buffer(Buffer), _offset(Offset), _size(Size) {}

            template <typename T>
            VoidPointer(Pointer<T> ptr) :
            _buffer(ptr._buffer), _offset(ptr._offset), _size(sizeof(T))
            {}

            wgpu::Buffer buffer() { return _buffer; }
            const wgpu::Buffer& buffer() const { return _buffer; }
            constexpr size_t offset() const { return _offset; }
            constexpr size_t size() const { return _size; }

            void write(const wgpu::Queue& queue, const void* data) const {
                queue.WriteBuffer(_buffer, _offset, data, _size);
            }
        };

        template <typename T>
        struct PointerWithQueue {
            wgpu::Buffer _buffer;
            wgpu::Queue _queue;
            size_t _offset;

            PointerWithQueue(
                const wgpu::Buffer& Buffer,
                const wgpu::Queue& Queue,
                size_t Offset
            ) : _buffer(Buffer), _queue(Queue), _offset(Offset) {}

            wgpu::Queue queue() { return _queue; }
            const wgpu::Queue& queue() const { return _queue; }
            wgpu::Buffer buffer() { return _buffer; }
            const wgpu::Buffer& buffer() const { return _buffer; }
            constexpr size_t offset() const { return _offset; }
            constexpr size_t size() const { return sizeof(T); }

            void write(const T& data) const {
                _queue.WriteBuffer(
                    _buffer,
                    _offset,
                    &data,
                    sizeof(T)
                );
            }
        };

        struct VoidPointerWithQueue {
            const wgpu::Buffer& _buffer;
            const wgpu::Queue& _queue;
            size_t _offset;
            size_t _size;

            VoidPointerWithQueue(
                const wgpu::Buffer& Buffer,
                const wgpu::Queue& Queue,
                size_t Offset, size_t Size
            ) : _buffer(Buffer), _queue(Queue), _offset(Offset), _size(Size) {}

            const wgpu::Queue& queue() const { return _queue; }
            const wgpu::Buffer& buffer() const { return _buffer; }
            constexpr size_t offset() const { return _offset; }
            constexpr size_t size() const { return _size; }

            void write(const void* data) const {
                _queue.WriteBuffer(
                    _buffer, _offset, data, _size
                );
            }

            template <typename T>
            VoidPointerWithQueue(Pointer<T> ptr) :
            _buffer(ptr._buffer), _offset(ptr._offset), _size(sizeof(T)) {}

            template <typename T>
            PointerWithQueue<T> typed() const {
                return PointerWithQueue<T>(
                    _buffer, _queue, _offset
                );
            }
        };

        template <typename T>
        constexpr ThinPointer<T>::ThinPointer(ThinVoidPointer ptr) : _offset(ptr.offset()) {
            assert(ptr.size() == sizeof(T));
        }

        template<typename T>
        constexpr ThinVoidPointer::ThinVoidPointer(ThinPointer<T> ptr) : _offset(ptr.offset()), _size(sizeof(T)) {}

        template <typename T>
        constexpr Pointer<T>::Pointer(
            const VoidPointer& ptr
        ) : _buffer(ptr.buffer()), _offset(ptr.offset()) {
            assert(ptr.size() == sizeof(T));
        }

    };

    struct Buffer {
        struct Allocation {
            const wgpu::Buffer& buffer;
            size_t offset;
            size_t size;
        };

        template <typename T>
        struct PointerWithQueue;

        template <typename T>
        struct Pointer {
            using element_type = T;

            const wgpu::Buffer& buffer;
            size_t offset;

            void write(const wgpu::Queue& queue, const T& data) const {
                queue.WriteBuffer(buffer, offset, &data, sizeof(T));
            }
            void write(const wgpu::Queue& queue, const T* data, size_t count = 1) const {
                queue.WriteBuffer(buffer, offset, data, count * sizeof(T));
            }

            Pointer& operator++() { offset += sizeof(T); }
            Pointer& operator++(int) { Pointer tmp = *this; offset += sizeof(T); return tmp; }

            Pointer& operator--() { offset -= sizeof(T); }
            Pointer& operator--(int) { Pointer tmp = *this; offset -= sizeof(T); return tmp; }

            PointerWithQueue<T> attachQueue(const wgpu::Queue& queue) const;
        };

        template <typename T>
        struct PointerWithQueue {
            const wgpu::Queue& queue;
            const wgpu::Buffer& buffer;
            size_t offset;

            void write(const T& data) const {
                queue.WriteBuffer(buffer, offset, &data, sizeof(T));
            }
            void write(const T* data, size_t count = 1) const {
                queue.WriteBuffer(buffer, offset, &data, sizeof(T) * count);
            }
        };

        template <typename T>
        static void write(
            const wgpu::Queue& queue, Pointer<T> dst,
            const T* data, size_t count = 1
        ) {
            dst.write(queue, data, count);
        }

    private:
        wgpu::Buffer _buffer;
        size_t _size;

    public:
        Buffer(
            const wgpu::Device& device,
            const wgpu::BufferDescriptor& desc
        ) : _buffer(device.CreateBuffer(&desc)), _size(desc.size) {}

        inline size_t size() const { return _size; }
    };

    template <typename T>
    inline Buffer::PointerWithQueue<T> Buffer::Pointer<T>::attachQueue(
        const wgpu::Queue& queue
    ) const {
        return {
            .queue = queue,
            .buffer = buffer,
            .offset = offset
        };
    }
}