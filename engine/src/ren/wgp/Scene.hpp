#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <rpg/ren/Scene.hpp>
#include "constmeshbuffer.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace rpg::ren::wgp {
    struct Scene {
        struct Instance {
            glm::mat4 M;
            uint32_t textureId;
        };
        struct DrawCall {
            uint32_t firstInstance = 0;
            uint32_t instanceCount = 0;
        };
        std::vector<Instance> objects;
        std::vector<DrawCall> calls;
        struct {
            glm::vec3 eye;
            glm::vec3 center;
            float fovy;
            float zNear;
            float zFar;
        } camera;

        Scene(const ren::Scene& scene) :
            objects(scene.entries.size()),
            calls(constmeshbuffer::MeshPointers.size()),
            camera({
                .eye = glm::vec3(
                    scene.camera.position.x,
                    scene.camera.position.y,
                    scene.camera.position.z
                ),
                .center = glm::vec3(
                    scene.camera.center.x,
                    scene.camera.center.y,
                    scene.camera.center.z
                ),
                .fovy = scene.camera.fov,
                .zNear = scene.camera.near,
                .zFar = scene.camera.far
            })
        {
            for (const auto& e : scene.entries) calls[e.meshId].instanceCount += 1;
            uint32_t offset = 0;
            std::vector<uint32_t> offsets(calls.size());
            for (size_t i = 0; i < calls.size(); ++i) {
                auto& c = calls[i];
                c.firstInstance = offset;
                offsets[i] = offset;
                offset += c.instanceCount;
            }
            for (size_t i = 0; i < scene.entries.size(); ++i) {
                const auto& e = scene.entries[i];
                auto& o = objects[offsets[e.meshId]++];
                o.M = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(e.position.x, e.position.y, e.position.z)
                );
                o.M *= glm::toMat4(glm::quat(glm::vec3(e.rotation.x, e.rotation.y, e.rotation.z)));
                o.M = glm::scale(o.M, glm::vec3(e.scale.x, e.scale.y, e.scale.z));
                o.textureId = e.materialId;
            }
        }
    };
}