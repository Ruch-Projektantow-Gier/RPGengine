#include <rpg/runGame.hpp>

int main() {
	using namespace rpg;
	using namespace rpg::math;
	ren::Scene scene;
	scene.entries.push_back(ren::Scene::Entry {
		.materialId = 0,
		.position = Vec3f(0.0f),
		.rotation = Vec3f(0.0f),
		.scale = Vec3f(1.0f)
	});
	scene.entries.push_back(ren::Scene::Entry {
		.materialId = 1,
		.position = Vec3f(0.0f, 0.0f, 4.0f),
		.rotation = Vec3f(0.0f),
		.scale = Vec3f(1.0f)
	});
	rpg::runGame(
		std::move(scene),
		[](ren::Scene& scene, float deltaTime) {
			scene.entries[0].rotation.y += deltaTime / 2.0f;
			scene.entries[1].rotation.y -= deltaTime / 2.0f;
		}
	);
}