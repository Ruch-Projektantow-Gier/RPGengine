#pragma once

#include <span>
#include <vector>
#include <glm/glm.hpp>
#include <rpg/ren/Scene.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace rpg::ren::wgp {
    struct MaterialBinding {
        uint32_t bindingId;
        uint32_t materialId;
    };
    struct Scene {
        struct Instance {
            glm::mat4 M;
            uint32_t textureId;
        };
        struct DrawCall {
            uint32_t meshId = 0;
            uint32_t firstInstance = 0;
            uint32_t instanceCount = 0;
        };
        struct Binding {
            uint32_t bindingId = 0;
            uint32_t firstDrawCall = 0;
            uint32_t drawCallCount = 0;
        };
        std::vector<Instance> objects;
        std::vector<DrawCall> calls;
        std::vector<Binding> bindings;
        struct {
            glm::vec3 eye;
            glm::vec3 center;
            float fovy;
            float zNear;
            float zFar;
        } camera;

        Scene(const ren::Scene& scene, std::span<const MaterialBinding> materialBindingMap) :
            objects(scene.objects.size()),
            calls(), bindings(),
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
            auto sorted = scene.objects;
            std::sort(sorted.begin(), sorted.end(), [materialBindingMap](
                const auto& a, const auto& b
            ) {
                uint32_t abind = materialBindingMap[a.materialId].bindingId;
                uint32_t bbind = materialBindingMap[b.materialId].bindingId;
                if (abind == bbind)
                    return a.meshId < b.meshId;
                return abind < bbind;
            });
            bindings.push_back({
                .bindingId = materialBindingMap[sorted[0].materialId].bindingId,
                .firstDrawCall = 0,
                .drawCallCount = 1
            });
            calls.push_back({
                .meshId = sorted[0].meshId,
                .firstInstance = 0,
                .instanceCount = 0
            });
            for (size_t i = 0; i < sorted.size(); ++i) {
                const auto& e = sorted[i];
                uint32_t bindingId = materialBindingMap[e.materialId].bindingId;
                if (bindingId != bindings.back().bindingId) {
                    bindings.push_back({
                        .bindingId = bindingId,
                        .firstDrawCall = static_cast<uint32_t>(calls.size()),
                        .drawCallCount = 1
                    });
                    calls.push_back({
                        .meshId = e.meshId,
                        .firstInstance = static_cast<uint32_t>(i),
                        .instanceCount = 1
                    });
                } else if (e.meshId != calls.back().meshId) {
                    bindings.back().drawCallCount += 1;
                    calls.push_back({
                        .meshId = e.meshId,
                        .firstInstance = static_cast<uint32_t>(i),
                        .instanceCount = 1
                    });
                } else {
                    calls.back().instanceCount += 1;
                }
            }
            for (size_t i = 0; i < sorted.size(); ++i) {
                const auto& e = sorted[i];
                auto& o = objects[i];
                o.M = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(e.position.x, e.position.y, e.position.z)
                );
                o.M *= glm::toMat4(glm::quat(glm::vec3(
                    e.rotation.x, e.rotation.y, e.rotation.z
                )));
                o.M = glm::scale(o.M, glm::vec3(e.scale.x, e.scale.y, e.scale.z));
                o.textureId = materialBindingMap[e.materialId].materialId;
            }
        }
    };
}