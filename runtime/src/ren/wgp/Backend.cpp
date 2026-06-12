#include <iostream>
#include <limits>
#include <array>

#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>
#include <dawn/webgpu_cpp_print.h>

#include <rpg/ren/wgp/Backend.hpp>
#include "BindGroup.hpp"

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

            wgpu::DeviceDescriptor desc { { .requiredLimits = &requiredLimits } };
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

    wgpu::BindGroupLayout makeBindGroupLayout(
        const wgpu::Device &device,
        const wgpu::BindGroupLayoutEntry& entry,
		std::string_view label = std::string_view()
    ) {
		wgpu::BindGroupLayoutDescriptor desc {
			.label = label,
            .entryCount = 1,
            .entries = &entry
        };
		return device.CreateBindGroupLayout(&desc);
    }
    wgpu::BindGroupLayout makeBindGroupLayout(
        const wgpu::Device &device,
        const std::vector<wgpu::BindGroupLayoutEntry>& entries,
		std::string_view label = std::string_view()
    ) {
		wgpu::BindGroupLayoutDescriptor desc {
			.label = label,
            .entryCount = entries.size(),
            .entries = entries.data()
        };
		return device.CreateBindGroupLayout(&desc);
    }

	wgpu::BindGroup makeBindGroup(
		const wgpu::Device& device,
		const wgpu::BindGroupLayout& layout,
		const wgpu::BindGroupEntry& entry,
		std::string_view label = std::string_view()
	){
		wgpu::BindGroupDescriptor desc {
			.label = label,
			.layout = layout,
			.entryCount = 1,
			.entries = &entry
		};
		return device.CreateBindGroup(&desc);
	};

	wgpu::BindGroup makeBindGroup(
		const wgpu::Device& device,
		const wgpu::BindGroupLayout& layout,
		const std::vector<wgpu::BindGroupEntry>& entries,
		std::string_view label = std::string_view()
	){
		wgpu::BindGroupDescriptor desc {
			.label = label,
			.layout = layout,
			.entryCount = entries.size(),
			.entries = entries.data()
		};
		return device.CreateBindGroup(&desc);
	};

    wgpu::PipelineLayout makePipelineLayout(
        const wgpu::Device& device,
        const auto& bindGroupLayouts
    ) {
        wgpu::PipelineLayoutDescriptor desc {
            .label = "Pipeline Layout Descriptor",
            .bindGroupLayoutCount = bindGroupLayouts.size(),
            .bindGroupLayouts = bindGroupLayouts.data(),
        };
	    return device.CreatePipelineLayout(&desc);
    }

	wgpu::ShaderModule makeShaderModule(const wgpu::Device& device){
		wgpu::ShaderSourceWGSL wgsl { { .code = R"(
			struct Uniforms {
				M: mat4x4f,
			};

			struct WorldUniforms {
				PV: mat4x4f,
			};

			@group(0) @binding(0) var<uniform> wu: WorldUniforms;
			@group(1) @binding(0) var<uniform> u: Uniforms;
			@group(1) @binding(1) var color: texture_2d<f32>;
			@group(1) @binding(2) var texsampler: sampler;

			struct Vertex {
				@location(0) position: vec3f,
				@location(1) normal: vec3f,
				@location(2) texcoord: vec2f,
			};

			struct Varyings {
				@builtin(position) position: vec4f,
				@location(0) normal: vec3f,
				@location(1) texcoord: vec2f,
			};

			@vertex fn vert(in: Vertex) -> Varyings {
				var out: Varyings;
				out.position = wu.PV * u.M * vec4f(in.position, 1.0);
				out.normal = (u.M * vec4f(in.normal, 0.0)).xyz;
				out.texcoord = in.texcoord;
				return out;
			}

			@fragment fn frag(in: Varyings) -> @location(0) vec4f {
			    let nl = max(dot(in.normal, normalize(vec3f(1, 1, 1))), 0.1);
			   	let color: vec4f = textureSample(color, texsampler, in.texcoord);
			  	return vec4f(color.rgb * nl, color.a);
			}
		)" } };
		wgpu::ShaderModuleDescriptor shaderModuleDescriptor { .nextInChain = &wgsl };
		return device.CreateShaderModule(&shaderModuleDescriptor);
	};

	wgpu::RenderPipeline makeRenderPipeline(
        const wgpu::Device& device,
        const wgpu::ShaderModule& shaderModule,
        const wgpu::PipelineLayout& pipelineLayout,
        wgpu::TextureFormat colorFormat
    ) {
		wgpu::BlendComponent blendComponent {
			.operation = wgpu::BlendOperation::Add,
			.srcFactor = wgpu::BlendFactor::SrcAlpha,
			.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha,
		};
		wgpu::BlendState blendState {
			.color = blendComponent,
			.alpha = blendComponent,
		};
		wgpu::ColorTargetState colorTargetState {
			.format = colorFormat,
			.blend = &blendState,
		};
		wgpu::FragmentState fragmentState {
			.module = shaderModule,
			.targetCount = 1,
			.targets = &colorTargetState
		};
		wgpu::DepthStencilState depthStencilState {
			.format = depthFormat,
			.depthWriteEnabled = true,
			.depthCompare = wgpu::CompareFunction::Less,
			.stencilReadMask = 0,
			.stencilWriteMask = 0,
		};
		std::array<wgpu::VertexAttribute, 3> attributes {
			wgpu::VertexAttribute {
				.format = wgpu::VertexFormat::Float32x3,
				.offset = 0,
				.shaderLocation = 0,
			},
			wgpu::VertexAttribute {
				.format = wgpu::VertexFormat::Float32x3,
				.offset = 3 * sizeof(float),
				.shaderLocation = 1,
			},
			wgpu::VertexAttribute {
				.format = wgpu::VertexFormat::Float32x2,
				.offset = 6 * sizeof(float),
				.shaderLocation = 2,
			},
		};
		wgpu::VertexBufferLayout vertexBufferLayout {
			.stepMode = wgpu::VertexStepMode::Vertex,
			.arrayStride = 8 * sizeof(float),
			.attributeCount = attributes.size(),
			.attributes = attributes.data(),
		};
		wgpu::RenderPipelineDescriptor descriptor {
			.layout = pipelineLayout,
			.vertex = {
				.module = shaderModule,
				.bufferCount = 1,
				.buffers = &vertexBufferLayout,
			},
			.primitive = {
				.topology = wgpu::PrimitiveTopology::TriangleList,
				.frontFace = wgpu::FrontFace::CCW,
				.cullMode = wgpu::CullMode::Back,
			},
			.depthStencil = &depthStencilState,
			.multisample = { .count = sampleCount, },
			.fragment = &fragmentState,
		};
		return device.CreateRenderPipeline(&descriptor);
	}

	wgpu::Buffer makeBuffer(const wgpu::Device& device, const wgpu::BufferDescriptor& desc) {
		return device.CreateBuffer(&desc);
	}

    Backend::BindGroupLayouts::BindGroupLayouts(const wgpu::Device& device) :
		world(BindGroup::Layout::make(
			device, "World Bind Group Layout",
			BindGroup::Layout::BufferEntry {
				BindGroup::Layout::BasicEntry {.visibility = wgpu::ShaderStage::Vertex },
				wgpu::BufferBindingLayout {
					.type = wgpu::BufferBindingType::Uniform,
					.minBindingSize = sizeof(glm::mat4)
				},
			}
		)),
		object(BindGroup::Layout::make(
			device, "Object Bind Group Layout",
			BindGroup::Layout::BufferEntry {
				{.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment },
				{
					.type = wgpu::BufferBindingType::Uniform,
					.hasDynamicOffset = true,
					.minBindingSize = sizeof(glm::mat4),
				}
			},
			BindGroup::Layout::TextureEntry {
				{ .visibility = wgpu::ShaderStage::Fragment },
				{
					.sampleType = wgpu::TextureSampleType::Float,
					.viewDimension = wgpu::TextureViewDimension::e2D,
				},
			},
			BindGroup::Layout::SamplerEntry {
				{ .visibility = wgpu::ShaderStage::Fragment },
				{ .type = wgpu::SamplerBindingType::Filtering },
			}
		)) {}
    
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
        bindGroupLayouts(device),
        pipelineLayout(makePipelineLayout(device, bindGroupLayouts)),
        shaderModule(makeShaderModule(device)),
        renderPipeline(makeRenderPipeline(device, shaderModule, pipelineLayout, colorFormat)),
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
		}(device)),
		meshes(device, queue), uniforms(device)
	{
        configureSurface(surface, device, colorFormat, width, height);

		wgpu::AdapterInfo props;
		if (adapter.GetInfo(&props)) {
			std::cout << "Running on: " << props.backendType << '\n';
		}
    }
    Backend::~Backend() {
        depthTexture.Destroy();
        multipleTexture.Destroy();
    }

	wgpu::BindGroup Backend::makeWorldBindGroup(
		size_t uniformBufferOffset,
		std::string_view label
	) const {
		return makeBindGroup(
			device,
			bindGroupLayouts.world,
			wgpu::BindGroupEntry {
				.binding = 0,
				.buffer = uniforms._buffer,
				.offset = uniformBufferOffset,
				.size = sizeof(glm::mat4),
			},
			label
		);
	}
	wgpu::BindGroup Backend::makeModelBindGroup(
		size_t uniformBufferOffset,
		const wgpu::TextureView& textureView,
		std::string_view label
	) const {
		return makeBindGroup(
			device,
			bindGroupLayouts.object,
			{
				wgpu::BindGroupEntry {
					.binding = 0,
					.buffer = uniforms._buffer,
					.offset = uniformBufferOffset,
					.size = sizeof(Uniform),
				},
				wgpu::BindGroupEntry {
					.binding = 1,
					.textureView = textureView,
				},
				wgpu::BindGroupEntry {
					.binding = 2,
					.sampler = sampler,
				},
			},
			label
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