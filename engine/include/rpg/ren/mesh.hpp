#pragma once
#include <array>
#include <rpg/math/Vec.hpp>

namespace rpg::ren::mesh {
    struct Vertex {
        math::Vec3f position;
        math::Vec3f normal;
        math::Vec2f texcoord;
    };
    template <size_t VertexCount, size_t IndexCount, typename IndexType = uint32_t>
    struct MeshData {
        std::array<Vertex, VertexCount> vertexBuffer;
        std::array<IndexType, IndexCount> indexBuffer;
    };
    template <size_t S1, size_t S2>
    constexpr std::array<Vertex, S1 + S2> concatVertexBuffer(
        const std::array<Vertex, S1>& a1, const std::array<Vertex, S2>& a2
    ) {
        std::array<Vertex, S1 + S2> res {};
        size_t j = 0;
        for (size_t i = 0; i < a1.size(); ++i) res[j++] = a1[i];
        for (size_t i = 0; i < a2.size(); ++i) res[j++] = a2[i];
        return res;
    }

    template <typename T, size_t S1, size_t S2>
    constexpr std::array<T, S1 + S2> concatIndexBuffer(
        const std::array<T, S1>& a1, const std::array<T, S2>& a2, size_t vertexBufferSize
    ) {
        std::array<T, S1 + S2> res;
        size_t j = 0;
        for (size_t i = 0; i < a1.size(); ++i) res[j++] = a1[i];
        for (size_t i = 0; i < a2.size(); ++i) res[j++] = a2[i] + vertexBufferSize;
        return res;
    }

    namespace plane {
        constexpr size_t vertexBufferSize = 4;
        constexpr size_t indexBufferSize = 6;
        constexpr std::array<Vertex, vertexBufferSize> vertexBuffer(
            math::Vec2f dimensions = math::Vec2f(1.0f),
            math::Vec3f offset = math::Vec3f(0.0f)
        ) {
            using namespace math;
            Vec3f normal = Vec3f(0.0f, 1.0f, 0.0f);
            return {
                Vertex {
                    Vec3f(dimensions.x, 0.0f, -dimensions.y) + offset,
                    normal, Vec2f(1.0f, 0.0f)
                },
                Vertex {
                    Vec3f(dimensions.x, 0.0f, dimensions.y) + offset,
                    normal, Vec2f(1.0f, 1.0f)
                },
                Vertex {
                    Vec3f(-dimensions.x, 0.0f, dimensions.y) + offset,
                    normal, Vec2f(0.0f, 1.0f)
                },
                Vertex {
                    Vec3f(-dimensions.x, 0.0f, -dimensions.y) + offset,
                    normal, Vec2f(0.0f, 0.0f)
                },
            };
        }
        template <typename T = uint32_t>
        inline constexpr std::array<T, indexBufferSize> indexBuffer { 0, 1, 2, 0, 2, 3 };
    }
    namespace cube {
        inline constexpr std::size_t vertexBufferSize = 24;
        inline constexpr std::array<Vertex, vertexBufferSize> vertexBuffer(
            math::Vec3f size = math::Vec3f(1.0f),
            math::Vec3f offset = math::Vec3f(0.0f)
        ) {
            using namespace math;
            std::array<math::Vec2f, 4> texcoords {
                Vec2f(0.0f, 0.0f),
                Vec2f(1.0f, 0.0f),
                Vec2f(1.0f, 1.0f),
                Vec2f(0.0f, 1.0f),
            };
            std::array<Vec3f, 6> normals {
                Vec3f(0.0f, 0.0f, -1.0f),
                Vec3f(0.0f, 0.0f, 1.0f),
                Vec3f(0.0f, -1.0f, 0.0f),
                Vec3f(0.0f, 1.0f, 0.0f),
                Vec3f(-1.0f, 0.0f, 0.0f),
                Vec3f(1.0f, 0.0f, 0.0f),
            };
            std::array<Vec3f, 8> positions {
                Vec3f(-size.x, -size.y, -size.z) + offset,
                Vec3f(-size.x, -size.y, size.z) + offset,
                Vec3f(-size.x, size.y, -size.z) + offset,
                Vec3f(-size.x,size.y, size.z) + offset,
                Vec3f(size.x, -size.y, -size.z) + offset,
                Vec3f(size.x, -size.y, size.z) + offset,
                Vec3f(size.x, size.y, -size.z) + offset,
                Vec3f(size.x, size.y, size.z) + offset,
            };
            return {
                Vertex { positions[4], normals[0], texcoords[0] },
                Vertex { positions[6], normals[0], texcoords[1] },
                Vertex { positions[2], normals[0], texcoords[2] },
                Vertex { positions[0], normals[0], texcoords[3] },

                Vertex { positions[1], normals[1], texcoords[0] },
                Vertex { positions[3], normals[1], texcoords[1] },
                Vertex { positions[7], normals[1], texcoords[2] },
                Vertex { positions[5], normals[1], texcoords[3] },

                Vertex { positions[0], normals[2], texcoords[0] },
                Vertex { positions[1], normals[2], texcoords[1] },
                Vertex { positions[5], normals[2], texcoords[2] },
                Vertex { positions[4], normals[2], texcoords[3] },

                Vertex { positions[6], normals[3], texcoords[0] },
                Vertex { positions[7], normals[3], texcoords[1] },
                Vertex { positions[3], normals[3], texcoords[2] },
                Vertex { positions[2], normals[3], texcoords[3] },

                Vertex { positions[2], normals[4], texcoords[0] },
                Vertex { positions[3], normals[4], texcoords[1] },
                Vertex { positions[1], normals[4], texcoords[2] },
                Vertex { positions[0], normals[4], texcoords[3] },

                Vertex { positions[4], normals[5], texcoords[0] },
                Vertex { positions[5], normals[5], texcoords[1] },
                Vertex { positions[7], normals[5], texcoords[2] },
                Vertex { positions[6], normals[5], texcoords[3] },
            };
        }
        inline constexpr std::size_t indexBufferSize = 36;
        template <typename T = uint32_t>
        inline static constexpr std::array<T, indexBufferSize> indexBuffer {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23,
        };
    }
    template <size_t Resolution>
    struct cylinder {
        constexpr static size_t vertexBufferSize = Resolution * 4 + 2;
        constexpr static std::array<Vertex, vertexBufferSize> vertexBuffer(
            float height = 0.0f,
            float radius = 1.0f,
            math::Vec3f offset = math::Vec3f(0.0f)
        ) {
            using namespace math;
            constexpr Handedness H = Handedness::Left;
            std::array<Vertex, vertexBufferSize> res {};
            size_t vert = 0;
            size_t topVert = Resolution * 2 + 2;
            size_t botVert = topVert + Resolution;
            for (size_t i = 0; i <= Resolution; ++i) {
                Vec3f axis = Vec3f(0.0f, 1.0f, 0.0f);
                float fraction = static_cast<float>(i) / Resolution;
                float angle = fraction * 2.0f * pi<float>;
                res[vert++] = {
                    rotate<H>(Vec3f(0.0f, height, radius), axis, angle) + offset,
                    rotate<H>(Vec3f(0.0f, 0.0f, 1.0f), axis, angle),
                    Vec2f(fraction, 1.0f)
                };
                res[vert++] = {
                    rotate<H>(Vec3f(0.0f, -height, radius), axis, angle) + offset,
                    rotate<H>(Vec3f(0.0f, 0.0f, 1.0f), axis, angle),
                    Vec2f(fraction, 0.0f)
                };

                if (i < Resolution){
                    res[topVert++] = {
                        rotate<H>(Vec3f(0.0f, height, radius), axis, angle) + offset,
                        rotate<H>(Vec3f(0.0f, 1.0f, 0.0f), axis, angle),
                        Vec2f(0.0f)
                    };
                    res[botVert++] = {
                        rotate<H>(Vec3f(0.0f, height, radius), axis, angle) + offset,
                        rotate<H>(Vec3f(0.0f, -1.0f, 0.0f), axis, angle),
                        Vec2f(0.0f)
                    };
                }
            }
            return res;
        }
        static constexpr size_t indexBufferSize = Resolution * 12;
        template <typename T = uint32_t>
        constexpr static std::array<T, indexBufferSize> indexBuffer = []() {
            std::array<T, indexBufferSize> res {};
            T vert = 0;
            T topVertBegin = Resolution * 2 + 2;
            T topVert = topVertBegin;
            T botVertBegin = topVertBegin + Resolution;
            T botVert = botVertBegin;
            size_t id = 0;
            size_t topId = Resolution * 6;
            size_t botId = topId + Resolution;
            for (size_t i = 0; i < Resolution; ++i) {
                res[id++] = vert;
                res[id++] = vert + 1;
                res[id++] = vert + 2;
                res[id++] = vert + 1;
                res[id++] = vert + 3;
                res[id++] = vert + 2;
                vert += 2;

                res[topId++] = topVertBegin;
                res[topId++] = topVert + 1;
                res[topId++] = topVert + 2;
                topVert += 1;

                res[botId++] = botVertBegin;
                res[botId++] = botVert + 1;
                res[botId++] = botVert + 2;
                botVert += 1;
            }
            return res;
        }();
    };
}
