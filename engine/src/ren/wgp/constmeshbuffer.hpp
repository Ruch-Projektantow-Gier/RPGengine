#pragma once

#include <webgpu/webgpu_cpp.h>
#include <rpg/ren/mesh.hpp>

namespace rpg::ren::wgp::constmeshbuffer {
    struct Pointer {
        size_t offset;
        uint32_t vertexCount;
        uint32_t indexCount;

        constexpr size_t vertexDataSize() const { return vertexCount * sizeof(mesh::Vertex); }
        constexpr size_t indexDataSize() const { return indexCount * sizeof(uint32_t); }
        constexpr size_t totalDataSize() const { return vertexDataSize() + indexDataSize(); }

        constexpr size_t vertexOffset() const { return offset; }
        constexpr size_t indexOffset() const { return offset + vertexDataSize(); }
        constexpr uint32_t firstIndex() const { return static_cast<uint32_t>(indexOffset() / sizeof(uint32_t)); }
        constexpr size_t nextOffset() const { return offset + vertexDataSize() + indexDataSize(); }
    };

    constexpr Pointer CubePointer = {
        .offset = 0,
        .vertexCount = static_cast<uint32_t>(mesh::cube::vertexBuffer().size()),
        .indexCount = static_cast<uint32_t>(mesh::cube::indexBuffer<uint32_t>.size())
    };
    constexpr Pointer CylinderPointer = {
        .offset = CubePointer.nextOffset(),
        .vertexCount = static_cast<uint32_t>(mesh::cylinder<24>::vertexBufferSize),
        .indexCount = static_cast<uint32_t>(mesh::cylinder<24>::indexBufferSize)
    };

    constexpr std::array<Pointer, 2> MeshPointers { CubePointer, CylinderPointer };

    inline wgpu::Buffer create(
        const wgpu::Device& device, const wgpu::Queue& queue,
        std::string_view label = std::string_view()
    ) {
        wgpu::BufferDescriptor desc {
            .label = label,
            .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst,
            .size = CubePointer.totalDataSize() + CylinderPointer.totalDataSize(),
            .mappedAtCreation = false,
        };
        wgpu::Buffer buffer = device.CreateBuffer(&desc);

        const auto write = [&buffer, &queue](
            Pointer p,
            std::span<const mesh::Vertex> vertexData,
            std::span<const uint32_t> indexData
        ) {
            queue.WriteBuffer(buffer, p.vertexOffset(),
                vertexData.data(),
                p.vertexDataSize()
            );
            queue.WriteBuffer(buffer,
                p.indexOffset(),
                indexData.data(),
                p.indexDataSize()
            );
        };

        write(
            CubePointer,
            mesh::cube::vertexBuffer(),
            mesh::cube::indexBuffer<uint32_t>
        );
        write(
            CylinderPointer,
            mesh::cylinder<24>::vertexBuffer(),
            mesh::cylinder<24>::indexBuffer<uint32_t>()
        );

        return buffer;
    }
}