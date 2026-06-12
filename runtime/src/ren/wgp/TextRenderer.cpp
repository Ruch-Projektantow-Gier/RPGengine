#include <rpg/ren/wgp/TextRenderer.hpp>

#include <string>
#include <array>

#include <webgpu/webgpu_cpp.h>
#define block_embeds
#ifndef block_embeds
#include <src/r/font/roboto.hpp>
#endif

namespace rpg::ren::wgp {
	inline const auto fontTextureData = Texture::textureGradient(256, 256);
    TextRenderer::TextRenderer(
        const wgpu::Device& device,
        const wgpu::Sampler& sampler,
        wgpu::TextureFormat colorFormat,
		StackBuffer::pointer InstanceBufferPointer,
		float AspectRatio
    ) : fontTexture(Texture::make(
        device,
#ifndef block_embeds
        font::width,
        font::height,
		font::data.data(),
#else
		256, 256,
		fontTextureData.data(),
#endif
        wgpu::TextureFormat::R8Unorm,
        "Font Texture Atlas"
    )), shaderModule([&device]() {
		wgpu::ShaderSourceWGSL wgsl { { .code = R"(
			const vertex_data = array<vec2f, 4>(
				vec2f(1,1),
				vec2f(0,1),
				vec2f(1,0),
				vec2f(0,0),
			);

			@group(0) @binding(0) var color: texture_2d<f32>;
			@group(0) @binding(1) var texsampler: sampler;

			struct Vertex {
				@location(0) position: vec2f,
				@location(1) size: vec2f,
				@location(2) texcoord: vec4f,
			};

			struct Varyings {
				@builtin(position) position: vec4f,
				@location(0) texcoord: vec2f,
			};

			@vertex fn vert(@builtin(vertex_index) in_vertex_index: u32, in: Vertex) -> Varyings {
				var out: Varyings;
				out.position = vec4f(vertex_data[in_vertex_index] * in.size + in.position, 0.5, 1.0);
				out.texcoord = (vertex_data[in_vertex_index] * in.texcoord.zw + in.texcoord.xy) * vec2f(1.0, 1.0/95.0);
				return out;
			}

			@fragment fn frag(in: Varyings) -> @location(0) vec4f {
			    return vec4f(1,1,1,textureSample(color, texsampler, in.texcoord).r);
			}
		)" } };
		wgpu::ShaderModuleDescriptor shaderModuleDescriptor { .nextInChain = &wgsl };
		return device.CreateShaderModule(&shaderModuleDescriptor);
    }()), bindGroupLayout([&device, &sampler](const Texture& fontTexture){
        std::array<wgpu::BindGroupLayoutEntry, 2> entries {
            wgpu::BindGroupLayoutEntry {
                .binding = 0,
                .visibility = wgpu::ShaderStage::Fragment,
                .texture {
                    .sampleType = wgpu::TextureSampleType::Float,
                    .viewDimension = wgpu::TextureViewDimension::e2D,
                }
            },
            wgpu::BindGroupLayoutEntry {
                .binding = 1,
                .visibility = wgpu::ShaderStage::Fragment,
                .sampler = { .type = wgpu::SamplerBindingType::Filtering }
            },
        };
		wgpu::BindGroupLayoutDescriptor desc {
			.label = "Text Bind Group Layout",
            .entryCount = entries.size(),
            .entries = entries.data()
        };
		return device.CreateBindGroupLayout(&desc);
    }(fontTexture)), pipelineLayout([&device](
        const wgpu::BindGroupLayout& bindGroupLayout
    ){
        wgpu::PipelineLayoutDescriptor desc {
            .label = "Pipeline Layout Descriptor",
            .bindGroupLayoutCount = 1,
            .bindGroupLayouts = &bindGroupLayout,
        };
        return device.CreatePipelineLayout(&desc);
    }(bindGroupLayout)), renderPipeline([&device, colorFormat](
        const wgpu::ShaderModule& shaderModule,
        const wgpu::PipelineLayout& pipelineLayout
    ){
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
		std::array<wgpu::VertexAttribute, 3> attributes {
			wgpu::VertexAttribute {
				.format = wgpu::VertexFormat::Float32x2,
				.offset = 0,
				.shaderLocation = 0,
			},
			wgpu::VertexAttribute {
				.format = wgpu::VertexFormat::Float32x2,
				.offset = 2 * sizeof(float),
				.shaderLocation = 1,
			},
			wgpu::VertexAttribute {
				.format = wgpu::VertexFormat::Float32x4,
				.offset = 4 * sizeof(float),
				.shaderLocation = 2,
			},
		};
		wgpu::VertexBufferLayout vertexBufferLayout {
			.stepMode = wgpu::VertexStepMode::Instance,
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
				.topology = wgpu::PrimitiveTopology::TriangleStrip,
				.cullMode = wgpu::CullMode::None, // For some reason triangle strip is made so that it generates triangles with alternating clockwiseness
			},
			.multisample = { .count = 1, },
			.fragment = &fragmentState,
		};
		return device.CreateRenderPipeline(&descriptor);
    }(shaderModule, pipelineLayout)), bindGroup([&device, &sampler](
		const wgpu::BindGroupLayout& bindGroupLayout,
		const wgpu::TextureView& fontTextureView
	){
		std::array<wgpu::BindGroupEntry, 2> entries = {
			wgpu::BindGroupEntry {
				.binding = 0,
				.textureView = fontTextureView,
			},
			wgpu::BindGroupEntry {
				.binding = 1,
				.sampler = sampler
			}
		};
		wgpu::BindGroupDescriptor desc {
			.label = "Text Bind Group",
			.layout = bindGroupLayout,
			.entryCount = entries.size(),
			.entries = entries.data()
		};
		return device.CreateBindGroup(&desc);
	}(bindGroupLayout, fontTexture.view)), instanceBufferPointer(
		InstanceBufferPointer
	), charCount(0), aspectRatio(AspectRatio) { }

	void TextRenderer::draw(
		const wgpu::Queue& queue,
		const std::string& text,
		glm::vec2 position,
		float scale
	) {
		std::vector<CharData> data(text.size());
		auto ptr = data.data();
		glm::vec2 s = glm::vec2(scale / aspectRatio, scale);
		for (char c : text) {
			const font::Glyph& glyph = font::glyphs[c - 32];
			*ptr = CharData {
				.position = position + (glm::vec2(glyph.x, glyph.y) * s),
				.size = glm::vec2(glyph.width, -glyph.height) * s,
				.texcoord = glm::vec4(0.0f, c - 32, glyph.width, glyph.height)
			};
			position.x += glyph.advance * s.x;
			++ptr;
		}
		instanceBufferPointer.write(queue, data.data(), charCount * sizeof(CharData), data.size() * sizeof(CharData));
		charCount += data.size();
	}

    void TextRenderer::draw(
		const wgpu::Device& device,
		const wgpu::Queue& queue,
		const wgpu::Surface& surface
	) {
		wgpu::SurfaceTexture surfaceTexture;
		surface.GetCurrentTexture(&surfaceTexture);
		wgpu::RenderPassColorAttachment colorAttachment{
			.view = surfaceTexture.texture.CreateView(),
			.loadOp = wgpu::LoadOp::Load,
			.storeOp = wgpu::StoreOp::Store
		};
		wgpu::RenderPassDescriptor renderpass{
			.colorAttachmentCount = 1,
			.colorAttachments = &colorAttachment,
		};
		wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
        pass.SetPipeline(renderPipeline);
		pass.SetVertexBuffer(0, instanceBufferPointer.buffer);
		pass.SetBindGroup(0, bindGroup);
		pass.Draw(4, charCount);
		pass.End();
		wgpu::CommandBuffer commands = encoder.Finish();
		queue.Submit(1, &commands);
		charCount = 0;
    }
}