#include <rpg/runGame.hpp>
#include <rpg/ren/texture.hpp>

int main() {
	using namespace rpg;
	using namespace rpg::math;
	using namespace rpg::ren;

	static constexpr const std::array<texture::Data<texture::RGBA8, 16, 16>, 2> TextureData = {
		texture::Data<texture::RGBA8, 16, 16>({255, 255, 255, 255}),
		texture::Data<texture::RGBA8, 16, 16>({255, 0, 0, 255}),
	};

	rpg::runGame(
		{
			.width = 16,
			.height = 16,
			.count = 2,
			.data = TextureData.data()
		},
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