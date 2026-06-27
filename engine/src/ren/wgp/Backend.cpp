#include <iostream>
#include <limits>
#include <array>

#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>
#include <dawn/webgpu_cpp_print.h>

#include <rpg/ren/mesh.hpp>
#include "Backend.hpp"
#include "constmeshbuffer.hpp"

namespace rpg::ren::wgp {
    glfw::Window& makeWindow(int width, int height) {
        glfw::Window::hint(glfw::ClientApi::NoApi);
        return glfw::createWindow(width, height, "WebGPU window");
    }

    wgpu::Instance makeInstance() {
        auto feature = wgpu::InstanceFeatureName::TimedWaitAny;
        wgpu::InstanceDescriptor instanceDesc{
            .requiredFeatureCount = 1,
            .requiredFeatures = &feature,
        };
        wgpu::Instance instance = wgpu::CreateInstance(&instanceDesc);
        if (!instance)  throw std::runtime_error("Could not initialize WebGPU instance!");
        return instance;
    }

	wgpu::Adapter makeAdapter(const wgpu::Instance& instance) {
        wgpu::Adapter adapter;

		wgpu::RequestAdapterOptions options {
			.powerPreference = wgpu::PowerPreference::HighPerformance
		};
		wgpu::Future f1 = instance.RequestAdapter(
			&options,
			wgpu::CallbackMode::WaitAnyOnly,
			[&adapter](
				wgpu::RequestAdapterStatus status,
				wgpu::Adapter a,
				wgpu::StringView message
			) {
				if (status != wgpu::RequestAdapterStatus::Success) {
					std::cout << "RequestAdapter: " << message << "\n";
					exit(0);
				}
				adapter = std::move(a);
			}
		);
		instance.WaitAny(f1, std::numeric_limits<uint64_t>::max());
        return adapter;
	}

    wgpu::Device makeDevice(const wgpu::Instance& instance, const wgpu::Adapter& adapter) {
        wgpu::Device device;
        {
            wgpu::Limits supportedLimits;
            if (!adapter.GetLimits(&supportedLimits))
                throw std::runtime_error("Could not get webgpu adapter supported limits\n");

            wgpu::Limits requiredLimits = {
                .maxBindGroups = 2,
                .maxDynamicUniformBuffersPerPipelineLayout = 1,
                .maxSampledTexturesPerShaderStage = 1,
                .maxSamplersPerShaderStage = 1,
                .maxUniformBuffersPerShaderStage = 1,
                .maxUniformBufferBindingSize = sizeof(glm::mat4),
                .minUniformBufferOffsetAlignment = supportedLimits.minUniformBufferOffsetAlignment,
                .minStorageBufferOffsetAlignment = supportedLimits.minStorageBufferOffsetAlignment,
                .maxVertexBuffers = 1,
                .maxBufferSize = maxBufferSize,
                .maxVertexAttributes = 3,
                .maxVertexBufferArrayStride = 8 * sizeof(float),
                .maxInterStageShaderVariables = 3,
            };

            wgpu::DeviceDescriptor desc { { .nextInChain = nullptr, .requiredLimits = &requiredLimits } };
            desc.SetUncapturedErrorCallback(
                [](
                    const wgpu::Device&,
                    wgpu::ErrorType errorType,
                    wgpu::StringView message
                ) {
                    std::cerr << "Error: " << errorType << ": " << message << '\n';
                }
            );
            wgpu::Future f2 = adapter.RequestDevice(
                &desc,
                wgpu::CallbackMode::WaitAnyOnly,
                [&device](
                    wgpu::RequestDeviceStatus status,
                    wgpu::Device d,
                    wgpu::StringView message
                ) {
                    if (status != wgpu::RequestDeviceStatus::Success) {
                        std::cerr << "RequestDeviceError: " << message << '\n';
                        exit(0);
                    }
                    device = std::move(d);
                }
            );
            instance.WaitAny(f2, std::numeric_limits<uint64_t>::max());
            return device;
        }
    }

	void configureSurface(
        const wgpu::Surface& surface,
        const wgpu::SurfaceConfiguration& config
    ) {
        surface.Configure(&config);
    }

	void configureSurface(
        const wgpu::Surface& surface, const wgpu::Device& device,
        wgpu::TextureFormat colorFormat,
        uint32_t width, uint32_t height
    ) {
		wgpu::SurfaceConfiguration config{
			.device = device,
			.format = colorFormat,
			.width = width,
			.height = height
		};
		surface.Configure(&config);
	};

    wgpu::TextureFormat getColorFormat(const wgpu::Surface& surface, const wgpu::Adapter& adapter) {
        wgpu::SurfaceCapabilities capabilities;
        if (surface.GetCapabilities(adapter, &capabilities) != wgpu::Status::Success)
            throw std::runtime_error("Could not get surfece capabilites\n");

        std::cout << "Present formats:\n";
        for (size_t i = 0; i < capabilities.formatCount; ++i)
            std::cout << capabilities.formats[i] << '\n';

		std::array<wgpu::TextureFormat, 3> preference {
			wgpu::TextureFormat::RGBA8Unorm,
			wgpu::TextureFormat::BGRA8Unorm,
			wgpu::TextureFormat::RGBA8Uint,
		};
		for (auto format : preference) {
			for (size_t i = 0; i < capabilities.formatCount; ++i)
				if (capabilities.formats[i] == format) return format;
		}
        return capabilities.formats[0];
    }

	wgpu::Texture makeDepthTexture(
        const wgpu::Device& device,
        uint32_t width, uint32_t height
    ){
		wgpu::TextureDescriptor desc {
			.label = "Depth Texture",
			.usage = wgpu::TextureUsage::RenderAttachment,
			.dimension = wgpu::TextureDimension::e2D,
			.size = {width, height, 1},
			.format = depthFormat,
			.mipLevelCount = 1,
			.sampleCount = sampleCount,
			.viewFormatCount = 1,
			.viewFormats = &depthFormat,
		};
		return device.CreateTexture(&desc);
    }

    wgpu::TextureView makeDepthTextureView(
        const wgpu::Texture& depthTexture
    ) {
		wgpu::TextureViewDescriptor desc {
			.label = "Depth Texture View",
			.format = depthFormat,
			.dimension = wgpu::TextureViewDimension::e2D,
			.baseMipLevel = 0,
			.baseArrayLayer = 0,
			.arrayLayerCount = 1,
			.aspect = wgpu::TextureAspect::DepthOnly,
		};
		return depthTexture.CreateView(&desc);
	};

	wgpu::Texture makeMultisampleTexture(
        const wgpu::Device& device,
        wgpu::TextureFormat colorFormat,
        uint32_t width, uint32_t height
    ) {
		wgpu::TextureDescriptor desc {
			.label = "Multisample Texture",
			.usage = wgpu::TextureUsage::RenderAttachment,
			.dimension = wgpu::TextureDimension::e2D,
			.size = {width, height, 1},
			.format = colorFormat,
			.mipLevelCount = 1,
			.sampleCount = sampleCount,
			.viewFormatCount = 1,
			.viewFormats = &colorFormat,
		};
		return device.CreateTexture(&desc);
	};

	wgpu::Buffer makeBuffer(const wgpu::Device& device, const wgpu::BufferDescriptor& desc) {
		return device.CreateBuffer(&desc);
	}

    Backend::Backend(uint32_t width, uint32_t height) :
        window(makeWindow(width, height)),
        instance(makeInstance()),
        adapter(makeAdapter(instance)),
        device(makeDevice(instance, adapter)),
		queue(device.GetQueue()),
        surface(wgpu::glfw::CreateSurfaceForWindow(instance, window)),
        colorFormat(getColorFormat(surface, adapter)),
        depthTexture(makeDepthTexture(device, width, height)),
        depthTextureView(makeDepthTextureView(depthTexture)),
        multipleTexture(makeMultisampleTexture(device, colorFormat, width, height)),
		litRenderer(device, colorFormat),
		sampler([](const wgpu::Device& device) {
			wgpu::SamplerDescriptor desc {
				.addressModeU = wgpu::AddressMode::Repeat,
				.addressModeV = wgpu::AddressMode::Repeat,
				.addressModeW = wgpu::AddressMode::Repeat,
				.magFilter = wgpu::FilterMode::Linear,
				.minFilter = wgpu::FilterMode::Linear,
				.mipmapFilter = wgpu::MipmapFilterMode::Linear,
				.lodMinClamp = 0.0f,
				.lodMaxClamp = 1.0f,
				.compare = wgpu::CompareFunction::Undefined,
				.maxAnisotropy = 1,
			};
			return device.CreateSampler(&desc);
		}(device)), meshBuffer(constmeshbuffer::create(
			device, queue, "Const Mesh Buffer"
		)), uniforms(device)
	{
        configureSurface(surface, device, colorFormat, width, height);

		wgpu::AdapterInfo props;
		if (adapter.GetInfo(&props)) {
			std::cout << "Running on: " << props.backendType << '\n';
		}
    }
    Backend::~Backend() {
        // depthTexture.Destroy();
        // multipleTexture.Destroy();
    }

	wgpu::BindGroup Backend::makeWorldBindGroup(
		size_t uniformBufferOffset,
		std::string_view label
	) const {
		return litRenderer.createWorldBindGroup(
			uniforms._buffer, uniformBufferOffset, label
		);
	}
	wgpu::BindGroup Backend::makeModelBindGroup(
		const wgpu::TextureView& textureView,
		std::string_view label
	) const {
		return litRenderer.createModelBindGroup(
			textureView, sampler, label
		);
	}

    void Backend::onScreenResized(uint32_t width, uint32_t height) {
		depthTexture.Destroy();
		depthTexture = makeDepthTexture(device, width, height);
		depthTextureView = makeDepthTextureView(depthTexture);
		multipleTexture.Destroy();
		multipleTexture = makeMultisampleTexture(device, colorFormat, width, height);
		configureSurface(surface, device, colorFormat, width, height);
    }
}