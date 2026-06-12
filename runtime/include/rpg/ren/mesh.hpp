#pragma once
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace rpg::ren::mesh {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
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
            glm::vec2 dimensions = glm::vec2(1.0f),
            glm::vec3 offset = glm::vec3(0.0f)
        ) {
            glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
            return {
                Vertex {
                    glm::vec3(dimensions.x, 0.0f, -dimensions.y) + offset,
                    normal, glm::vec2(1.0f, 0.0f)
                },
                Vertex {
                    glm::vec3(dimensions.x, 0.0f, dimensions.y) + offset,
                    normal, glm::vec2(1.0f, 1.0f)
                },
                Vertex {
                    glm::vec3(-dimensions.x, 0.0f, dimensions.y) + offset,
                    normal, glm::vec2(0.0f, 1.0f)
                },
                Vertex {
                    glm::vec3(-dimensions.x, 0.0f, -dimensions.y) + offset,
                    normal, glm::vec2(0.0f, 0.0f)
                },
            };
        }
        template <typename T = uint32_t>
        inline constexpr std::array<T, indexBufferSize> indexBuffer { 0, 1, 2, 0, 2, 3 };
    }
    namespace cube {
        inline constexpr std::array<Vertex, 24> vertexBuffer(
            glm::vec3 size = glm::vec3(1.0f),
            glm::vec3 offset = glm::vec3(0.0f)
        ) {
            std::array<glm::vec2, 4> texcoords {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
            };
            std::array<glm::vec3, 6> normals {
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
            };
            std::array<glm::vec3, 8> positions {
                glm::vec3(-size.x, -size.y, -size.z) + offset,
                glm::vec3(-size.x, -size.y, size.z) + offset,
                glm::vec3(-size.x, size.y, -size.z) + offset,
                glm::vec3(-size.x,size.y, size.z) + offset,
                glm::vec3(size.x, -size.y, -size.z) + offset,
                glm::vec3(size.x, -size.y, size.z) + offset,
                glm::vec3(size.x, size.y, -size.z) + offset,
                glm::vec3(size.x, size.y, size.z) + offset,
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
        template <typename T = uint32_t>
        inline static constexpr std::array<T, 36> indexBuffer {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23,
        };
    }
    namespace sphere {
        template <size_t VerticalResolution, size_t HorizontalResolution>
        inline constexpr auto vertexBuffer(float radius = 1.0f) {
            std::array<Vertex, VerticalResolution * HorizontalResolution> res;
            for (size_t i = 0; i < HorizontalResolution; ++i) {
                float fraction = static_cast<float>(i) / HorizontalResolution;
                glm::quat rot = glm::angleAxis(
                    fraction * 2.0f * glm::pi<float>(),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );
                for (size_t j = 0; j < VerticalResolution; ++j) {
                    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f) * rot;
                    res[i * VerticalResolution + j] = {
                        normal * radius, normal, glm::vec2(0.0f)
                    };
                    float fraction = static_cast<float>(j) / VerticalResolution;
                    rot *= glm::angleAxis(
                        fraction * glm::pi<float>(),
                        glm::vec3(1.0f, 0.0f, 0.0f)
                    );
                }
            }
            return res;
        }
    }
    template <size_t Resolution>
    struct cylinder {
        constexpr static size_t vertexBufferSize = Resolution * 4 + 2;
        constexpr static std::array<Vertex, vertexBufferSize> vertexBuffer(
            float height = 0.0f,
            float radius = 1.0f,
            glm::vec3 offset = glm::vec3(0.0f)
        ) {
            std::array<Vertex, vertexBufferSize> res {};
            size_t vert = 0;
            size_t topVert = Resolution * 2 + 2;
            size_t botVert = topVert + Resolution;
            for (size_t i = 0; i <= Resolution; ++i) {
                float fraction = static_cast<float>(i) / Resolution;
                glm::quat rot = glm::angleAxis(
                    fraction * 2.0f * glm::pi<float>(),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );
                res[vert++] = {
                    glm::vec3(0.0f, height, radius) * rot + offset,
                    glm::vec3(0.0f, 0.0f, 1.0f) * rot,
                    glm::vec2(fraction, 1.0f)
                };
                res[vert++] = {
                    glm::vec3(0.0f, -height, radius) * rot + offset,
                    glm::vec3(0.0f, 0.0f, 1.0f) * rot,
                    glm::vec2(fraction, 0.0f)
                };

                if (i < Resolution){
                    res[topVert++] = {
                        glm::vec3(0.0f, height, radius) * rot + offset,
                        glm::vec3(0.0f, 1.0f, 0.0f),
                        glm::vec2(0.0f)
                    };
                    res[botVert++] = {
                        glm::vec3(0.0f, height, radius) * rot + offset,
                        glm::vec3(0.0f, -1.0f, 0.0f),
                        glm::vec2(0.0f)
                    };
                }
            }
            return res;
        }
        static constexpr size_t indexBufferSize = Resolution * 12;
        template <typename T = uint32_t>
        constexpr static std::array<T, indexBufferSize> indexBuffer() {
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
        }
    };
}
