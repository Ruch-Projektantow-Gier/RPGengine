#include "MipMapCalculator.hpp"

#include "BindGroup.hpp"
#include "ShaderModule.hpp"
#include "texture.hpp"

namespace rpg::ren::wgp {
    MipMapCalculator::MipMapCalculator(const wgpu::Device& device) : 
        bindGroupLayout(bindgroup::layout::create(device, {
            wgpu::BindGroupLayoutEntry {
                .binding = 0,
                .visibility = wgpu::ShaderStage::Compute,
                .texture = {
                    .sampleType = wgpu::TextureSampleType::Float,
                    .viewDimension = wgpu::TextureViewDimension::e2DArray,
                }
            },
            wgpu::BindGroupLayoutEntry {
                .binding = 1,
                .visibility = wgpu::ShaderStage::Compute,
                .storageTexture = {
                    .access = wgpu::StorageTextureAccess::WriteOnly,
                    .format = wgpu::TextureFormat::RGBA8Unorm,
                    .viewDimension = wgpu::TextureViewDimension::e2DArray
                }
            },
        })),
        pipeline([this, &device](){
            wgpu::ShaderModule shader = shadermodule::create(device, R"(
                @group(0) @binding(0)
                var srcTex : texture_2d_array<f32>;

                @group(0) @binding(1)
                var dstTex : texture_storage_2d_array<rgba8unorm, write>;

                @compute @workgroup_size(8, 8, 1)
                fn main(@builtin(global_invocation_id) id : vec3<u32>) {
                    let size = textureDimensions(dstTex);
                    if (id.x >= size.x || id.y >= size.y) {
                        return;
                    }

                    let dst = vec2<i32>(id.xy);
                    let layer = i32(id.z);

                    let base = dst * 2;

                    let a = textureLoad(srcTex, base, layer, 0);
                    let b = textureLoad(srcTex, base + vec2(1,0), layer, 0);
                    let c = textureLoad(srcTex, base + vec2(0,1), layer, 0);
                    let d = textureLoad(srcTex, base + vec2(1,1), layer, 0);

                    textureStore(dstTex, dst, layer, (a+b+c+d) * 0.25);
                }
            )");

            wgpu::PipelineLayoutDescriptor pipelineLayoutDescriptor = {
                .label = "Mip Map Compute Pipeline",
                .bindGroupLayoutCount = 1,
                .bindGroupLayouts = &bindGroupLayout,
            };
            wgpu::PipelineLayout pipelineLayout = device.CreatePipelineLayout(
                &pipelineLayoutDescriptor
            );

            wgpu::ComputePipelineDescriptor desc {
                .label = "Mip Map Compute Pipeline",
                .layout = pipelineLayout,
                .compute = {
                    .module = shader,
                    .entryPoint = "main",
                },
            };
            return device.CreateComputePipeline(&desc);
        }())
    {}

    void MipMapCalculator::calculateMipMaps(
        const wgpu::Device& device,
        const wgpu::Queue& queue,
        const wgpu::Texture& texture
    ) const {
        uint32_t mipLevelCount = texture.GetMipLevelCount();
        if (mipLevelCount == 1) return;
        uint32_t width = texture.GetWidth();
        uint32_t height = texture.GetHeight();
        uint32_t layers = texture.GetDepthOrArrayLayers();
        std::vector<wgpu::TextureView> mipViews;
        mipViews.reserve(mipLevelCount);
        wgpu::TextureFormat format = texture.GetFormat();
        for (uint32_t i = 0; i < mipLevelCount; ++i) {
            mipViews.push_back(wgp::texture::view::create(texture, {
                .label = "Mip Map View",
                .format = format,
                .dimension = wgpu::TextureViewDimension::e2DArray,
                .baseMipLevel = i,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = layers,
                .aspect = wgpu::TextureAspect::All,
            }));
        }
        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        wgpu::ComputePassEncoder pass = encoder.BeginComputePass();
        pass.SetPipeline(pipeline);
        for (uint32_t i = 1; i < mipLevelCount; ++i) {
            wgpu::BindGroup bindGroup = bindgroup::create(
                device,
                bindGroupLayout,
                {
                    wgpu::BindGroupEntry {
                        .binding = 0,
                        .textureView = mipViews[i-1]
                    },
                    wgpu::BindGroupEntry {
                        .binding = 1,
                        .textureView = mipViews[i]
                    }
                }
            );
            pass.SetBindGroup(0, bindGroup);
            pass.DispatchWorkgroups(
                (width + 7) / 8,
                (height + 7) / 8,
                layers
            );
            width = std::max(width / 2, 1u);
            height = std::max(height / 2, 1u);
        }
        pass.End();
        wgpu::CommandBuffer commands = encoder.Finish();
        queue.Submit(1, &commands);
    }
}