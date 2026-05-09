#include <memory>
#include <vector>

namespace rpg::ecs {
    class SystemBase {
    public:
        virtual void add() = 0;
        virtual void onUpdate() = 0;
    };

    template <typename T>
    struct System : SystemBase {
        std::vector<T> components;

        void add() override {
            components.push_back(T());
            components.back().onInit();
        }
        void onUpdate() override {
            for (auto& component : components) {
                component.onUpdate();
            }
        }
    };

    class ECS {
    public:
        std::vector<std::unique_ptr<SystemBase>> systems;
        void onUpdate() {
            for (auto& system : systems) {
                system->onUpdate();
            }
        }
    };
}