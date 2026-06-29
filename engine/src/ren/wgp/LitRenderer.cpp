#include "LitRenderer.hpp"

#include "ShaderModule.hpp"
#include "SampleCount.hpp"
#include "DepthFormat.hpp"
#include "BindGroup.hpp"

namespace rpg::ren::wgp {
    LitRenderer::LitRenderer(
        const wgpu::Device& Device,
        wgpu::TextureFormat colorFormat
    ) : device(Device), bindGroupLayouts{
        .world = bindgroup::layout::create(device, wgpu::BindGroupLayoutEntry {
            .binding = 0,
            .visibility = wgpu::ShaderStage::Vertex,
            .buffer {
                .type = wgpu::BufferBindingType::Uniform,
                .minBindingSize = WorldBindingSize
            },
        }, "LitRenderer World Bind Group Layout"),
        .object = bindgroup::layout::create(device, {
            wgpu::BindGroupLayoutEntry {
                .binding = 1,
                .visibility = wgpu::ShaderStage::Fragment,
                .texture {
                    .sampleType = wgpu::TextureSampleType::Float,
                    .viewDimension = wgpu::TextureViewDimension::e2DArray,
                }
            }, wgpu::BindGroupLayoutEntry {
                .binding = 2,
                .visibility = wgpu::ShaderStage::Fragment,
                .sampler { .type = wgpu::SamplerBindingType::Filtering }
            }
        }, "LitRenderer Object Bind Group Layout")
    }, pipeline([this, colorFormat](){
        wgpu::PipelineLayoutDescriptor desc {
            .label = "Pipeline Layout Descriptor",
            .bindGroupLayoutCount = 2,
            .bindGroupLayouts = bindGroupLayouts.data(),
        };
	    wgpu::PipelineLayout pipelineLayout = device.CreatePipelineLayout(&desc);

        wgpu::ShaderModule shaderModule = shadermodule::create(device, R"(
			struct WorldUniforms {
				PV: mat4x4f,
			};

			@group(0) @binding(0) var<uniform> wu: WorldUniforms;
			@group(1) @binding(1) var color: texture_2d_array<f32>;
			@group(1) @binding(2) var texsampler: sampler;

			struct Vertex {
				@location(0) position: vec3f,
				@location(1) normal: vec3f,
				@location(2) texcoord: vec2f,

				@location(3) M0: vec4f,
				@location(4) M1: vec4f,
				@location(5) M2: vec4f,
				@location(6) M3: vec4f,

				@location(7) tex1id: u32,
			};

			struct Varyings {
				@builtin(position) position: vec4f,
				@location(0) normal: vec3f,
				@location(1) texcoord: vec2f,
				@interpolate(flat) @location(2) tex1id: u32,
			};

			@vertex fn vert(in: Vertex) -> Varyings {
				var out: Varyings;
                let M = mat4x4f(in.M0, in.M1, in.M2, in.M3);
				out.position = wu.PV * M * vec4f(in.position, 1.0);
				out.normal = (M * vec4f(in.normal, 0.0)).xyz;
				out.texcoord = in.texcoord;
				out.tex1id = in.tex1id;
				return out;
			}

			@fragment fn frag(in: Varyings) -> @location(0) vec4f {
			    let nl = max(dot(in.normal, normalize(vec3f(1, 1, 1))), 0.1);
			   	let color: vec4f = textureSample(color, texsampler, in.texcoord, in.tex1id);
			  	return vec4f(color.rgb * nl, color.a);
			}
		)");

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
			.format = DepthFormat,
			.depthWriteEnabled = true,
			.depthCompare = wgpu::CompareFunction::Less,
			.stencilReadMask = 0,
			.stencilWriteMask = 0,
		};
		wgpu::VertexAttribute vertexAttributes[3] {
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
		wgpu::VertexAttribute instanceAttributes[5] {
			{
				.format = wgpu::VertexFormat::Float32x4,
				.offset = 0,
				.shaderLocation = 3,
			},
			{
				.format = wgpu::VertexFormat::Float32x4,
				.offset = 4 * sizeof(float),
				.shaderLocation = 4,
			},
			{
				.format = wgpu::VertexFormat::Float32x4,
				.offset = 8 * sizeof(float),
				.shaderLocation = 5,
			},
			{
				.format = wgpu::VertexFormat::Float32x4,
				.offset = 12 * sizeof(float),
				.shaderLocation = 6,
			},
			{
				.format = wgpu::VertexFormat::Uint32,
				.offset = 16 * sizeof(float),
				.shaderLocation = 7,
			},
		};
		wgpu::VertexBufferLayout vertexBufferLayouts[2] {
            {
                .stepMode = wgpu::VertexStepMode::Vertex,
                .arrayStride = 8 * sizeof(float),
                .attributeCount = 3,
                .attributes = vertexAttributes,
            },
            {
                .stepMode = wgpu::VertexStepMode::Instance,
                .arrayStride = InstanceSize,
                .attributeCount = 5,
                .attributes = instanceAttributes,
            },
		};
		wgpu::RenderPipelineDescriptor descriptor {
			.layout = pipelineLayout,
			.vertex = {
				.module = shaderModule,
				.bufferCount = 2,
				.buffers = vertexBufferLayouts,
			},
			.primitive = {
				.topology = wgpu::PrimitiveTopology::TriangleList,
				.frontFace = wgpu::FrontFace::CCW,
				.cullMode = wgpu::CullMode::Back,
			},
			.depthStencil = &depthStencilState,
			.multisample = { .count = SampleCount, },
			.fragment = &fragmentState,
		};
		return device.CreateRenderPipeline(&descriptor);
    }()) {}

    wgpu::BindGroup LitRenderer::createWorldBindGroup(
        const wgpu::Buffer& uniformBuffer,
        size_t uniformBufferOffset,
        std::string_view label
    ) const {
		return bindgroup::create(
			device,
			bindGroupLayouts.world,
			wgpu::BindGroupEntry {
				.binding = 0,
				.buffer = uniformBuffer,
				.offset = uniformBufferOffset,
				.size = WorldBindingSize,
			},
			label
		);
    }
    wgpu::BindGroup LitRenderer::createModelBindGroup(
        const wgpu::TextureView textureView,
        const wgpu::Sampler sampler,
        std::string_view label
    ) const {
		return bindgroup::create(
			device,
			bindGroupLayouts.object,
			{
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
}