#include <rpg/runGame.hpp>
#include <rpg/ren/texture.hpp>
#include <rpg/math/radians.hpp>
#define EMBEDDED_TEXTURE
#ifdef EMBEDDED_TEXTURE
#include "embeddedTexture.hpp"
#endif

int main() {
	using namespace rpg;
	using namespace rpg::math;
	using namespace rpg::ren;

	std::array<RGBA8, 3 * 3> textureData = {
		RGBA8 { 0, 0, 0, 255 }, 
		RGBA8 { 127, 0, 0, 255 }, 
		RGBA8 { 255, 0, 0, 255 }, 
		RGBA8 { 0, 127, 0, 255 }, 
		RGBA8 { 127, 127, 0, 255 }, 
		RGBA8 { 255, 127, 0, 255 }, 
		RGBA8 { 0, 255, 0, 255 }, 
		RGBA8 { 127, 255, 0, 255 }, 
		RGBA8 { 255, 255, 0, 255 }, 
	};

	texture::DataSource textureSources[] = {
		texture::datasource::SolidColor {
			.color = { 255, 255, 255, 255 }
		},
		texture::datasource::SolidColor {
			.color = { 255, 0, 0, 255 }
		},
		texture::datasource::RawData {
			.width = 3,
			.height = 3,
			.data = textureData.data()
		},
	#ifdef EMBEDDED_TEXTURE
		texture::datasource::EncodedData {
			.data = embeddedTexture.data(),
			.size = embeddedTexture.size()
		},
	#else
		texture::datasource::File {
			.filename = "texture.png"
		}
	#endif
	};
	Material materials[] = {
		{ .textureId = 0 },
		{ .textureId = 1 },
		{ .textureId = 2 },
		{ .textureId = 3 },
	};
	rpg::runGame(
		{
			.maxObjects = 10,
			.textureData = textureSources,
			.materials = materials
		},
		{
			.objects = {
				{
					.materialId = 0,
					.meshId = 0,
					.position = Vec3f(-4.0f, 0.0f, -4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 1,
					.meshId = 1,
					.position = Vec3f(0.0f, 0.0f, -4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 2,
					.meshId = 0,
					.position = Vec3f(4.0f, 0.0f, -4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 3,
					.meshId = 1,
					.position = Vec3f(-4.0f, 0.0f, 0.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 0,
					.meshId = 1,
					.position = Vec3f(0.0f, 0.0f, 0.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 1,
					.meshId = 0,
					.position = Vec3f(4.0f, 0.0f, 0.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 2,
					.meshId = 1,
					.position = Vec3f(-4.0f, 0.0f, 4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 3,
					.meshId = 0,
					.position = Vec3f(0.0f, 0.0f, 4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				},
				{
					.materialId = 0,
					.meshId = 0,
					.position = Vec3f(4.0f, 0.0f, 4.0f),
					.rotation = Vec3f(0.0f),
					.scale = Vec3f(1.0f)
				}
			},
			.camera = {
				.position = Vec3f(5.0f),
				.center = Vec3f(0.0f),
				.fov = math::radians(60.0f),
				.near = 0.1f,
				.far = 20.0f
			}
		},
		[](ren::Scene& scene, float deltaTime) {
			scene.objects[0].rotation.y += deltaTime / 2.0f;
			scene.objects[1].rotation.y -= deltaTime / 2.0f;
		}
	);
}