#include <rpg/runGame.hpp>

int main() {
	using namespace rpg;
	using namespace rpg::math;

	rpg::runGame(
		{
			.entries = {
				{
					.materialId = 0,
					.position = Vec3f(0.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 1,
					.position = Vec3f(0.0f, 0.0f, 4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				}
			}
		},
		[](ren::Scene& scene, float deltaTime) {
			scene.entries[0].rotation.y += deltaTime / 2.0f;
			scene.entries[1].rotation.y -= deltaTime / 2.0f;
		}
	);
}