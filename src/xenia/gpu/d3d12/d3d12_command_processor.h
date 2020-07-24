/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2018 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_GPU_D3D12_D3D12_COMMAND_PROCESSOR_H_
#define XENIA_GPU_D3D12_D3D12_COMMAND_PROCESSOR_H_

#include <deque>
#include <memory>
#include <unordered_map>

#include "xenia/gpu/command_processor.h"
#include "xenia/gpu/d3d12/d3d12_graphics_system.h"
#include "xenia/gpu/d3d12/pipeline_cache.h"
#include "xenia/gpu/d3d12/render_target_cache.h"
#include "xenia/gpu/d3d12/shared_memory.h"
#include "xenia/gpu/d3d12/texture_cache.h"
#include "xenia/gpu/hlsl_shader_translator.h"
#include "xenia/gpu/xenos.h"
#include "xenia/kernel/kernel_state.h"
#include "xenia/ui/d3d12/command_list.h"
#include "xenia/ui/d3d12/d3d12_context.h"
#include "xenia/ui/d3d12/pools.h"

namespace xe {
namespace gpu {
namespace d3d12 {

class D3D12CommandProcessor : public CommandProcessor {
 public:
  explicit D3D12CommandProcessor(D3D12GraphicsSystem* graphics_system,
                                 kernel::KernelState* kernel_state);
  ~D3D12CommandProcessor();

  void ClearCaches() override;

  // Needed by everything that owns transient objects.
  xe::ui::d3d12::D3D12Context* GetD3D12Context() const {
    return static_cast<xe::ui::d3d12::D3D12Context*>(context_.get());
  }

  // Returns the drawing command list for the currently open frame.
  ID3D12GraphicsCommandList* GetCurrentCommandList() const;

  // Finds or creates root signature for a pipeline.
  ID3D12RootSignature* GetRootSignature(const D3D12Shader* vertex_shader,
                                        const D3D12Shader* pixel_shader);

  ui::d3d12::UploadBufferPool* GetConstantBufferPool() const {
    return constant_buffer_pool_.get();
  }
  // Request and automatically rebind descriptors on the draw command list.
  // Refer to DescriptorHeapPool::Request for partial/full update explanation.
  uint64_t RequestViewDescriptors(uint64_t previous_full_update,
                                  uint32_t count_for_partial_update,
                                  uint32_t count_for_full_update,
                                  D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle_out,
                                  D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle_out);
  uint64_t RequestSamplerDescriptors(
      uint64_t previous_full_update, uint32_t count_for_partial_update,
      uint32_t count_for_full_update,
      D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle_out,
      D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle_out);

  // Returns a single temporary GPU-side buffer within a frame for tasks like
  // texture untiling and resolving.
  ID3D12Resource* RequestScratchGPUBuffer(uint32_t size,
                                          D3D12_RESOURCE_STATES state);
  // This must be called when done with the scratch buffer, to notify the
  // command processor about the new state in case the buffer was transitioned
  // by its user.
  void ReleaseScratchGPUBuffer(ID3D12Resource* buffer,
                               D3D12_RESOURCE_STATES new_state);

  // Sets the current pipeline state - may be called internally or externally.
  // This is for cache invalidation primarily. A frame must be open.
  void SetPipeline(ID3D12PipelineState* pipeline);

 protected:
  bool SetupContext() override;
  void ShutdownContext() override;

  void WriteRegister(uint32_t index, uint32_t value) override;

  void PerformSwap(uint32_t frontbuffer_ptr, uint32_t frontbuffer_width,
                   uint32_t frontbuffer_height) override;

  Shader* LoadShader(ShaderType shader_type, uint32_t guest_address,
                     const uint32_t* host_address,
                     uint32_t dword_count) override;

  bool IssueDraw(PrimitiveType primitive_type, uint32_t index_count,
                 IndexBufferInfo* index_buffer_info) override;
  bool IssueCopy() override;

 private:
  enum RootParameter : UINT {
    // These are always present.

    // Very frequently changed, especially for UI draws, and for models drawn in
    // multiple parts - contains vertex and texture fetch constants (b2).
    kRootParameter_FetchConstants,
    // Quite frequently changed (for one object drawn multiple times, for
    // instance - may contain projection matrices) - 8 pages of float constants
    // (b3-b10).
    kRootParameter_VertexFloatConstants,
    // Less frequently changed (per-material) - 8 pages of float constants
    // (b3-b10).
    kRootParameter_PixelFloatConstants,
    // Rarely changed - system constants like viewport and alpha testing (b0)
    // and loop and bool constants (b1).
    kRootParameter_CommonConstants,
    // Never changed - shared memory byte address buffer (t0, space1).
    kRootParameter_SharedMemory,

    kRootParameter_Count_Base,

    // Extra parameter that may or may not exist:
    // - Pixel textures.
    // - Pixel samplers.
    // - Vertex textures.
    // - Vertex samplers.

    kRootParameter_Count_Max = kRootParameter_Count_Base + 4,
  };

  struct RootExtraParameterIndices {
    uint32_t pixel_textures;
    uint32_t pixel_samplers;
    uint32_t vertex_textures;
    uint32_t vertex_samplers;
    static constexpr uint32_t kUnavailable = UINT32_MAX;
  };
  // Gets the indices of optional root parameters. Returns the total parameter
  // count.
  static uint32_t GetRootExtraParameterIndices(
      const D3D12Shader* vertex_shader, const D3D12Shader* pixel_shader,
      RootExtraParameterIndices& indices_out);

  // Returns true if a new frame was started.
  bool BeginFrame();
  // Returns true if an open frame was ended.
  bool EndFrame();

  void UpdateFixedFunctionState(ID3D12GraphicsCommandList* command_list);
  void UpdateSystemConstantValues(
      Endian index_endian,
      const RenderTargetCache::PipelineRenderTarget render_targets[4]);
  bool UpdateBindings(ID3D12GraphicsCommandList* command_list,
                      const D3D12Shader* vertex_shader,
                      const D3D12Shader* pixel_shader,
                      ID3D12RootSignature* root_signature);

  bool cache_clear_requested_ = false;

  std::unique_ptr<ui::d3d12::CommandList>
  command_lists_[ui::d3d12::D3D12Context::kQueuedFrames] = {};

  std::unique_ptr<SharedMemory> shared_memory_ = nullptr;

  // Root signatures for different descriptor counts.
  std::unordered_map<uint32_t, ID3D12RootSignature*> root_signatures_;

  std::unique_ptr<PipelineCache> pipeline_cache_ = nullptr;

  std::unique_ptr<TextureCache> texture_cache_ = nullptr;

  std::unique_ptr<RenderTargetCache> render_target_cache_ = nullptr;

  std::unique_ptr<ui::d3d12::UploadBufferPool> constant_buffer_pool_ = nullptr;
  std::unique_ptr<ui::d3d12::DescriptorHeapPool> view_heap_pool_ = nullptr;
  std::unique_ptr<ui::d3d12::DescriptorHeapPool> sampler_heap_pool_ = nullptr;

  struct BufferForDeletion {
    ID3D12Resource* buffer;
    uint64_t last_usage_frame;
  };
  std::deque<BufferForDeletion> buffers_for_deletion_;

  static constexpr uint32_t kScratchBufferSizeIncrement = 16 * 1024 * 1024;
  ID3D12Resource* scratch_buffer_ = nullptr;
  uint32_t scratch_buffer_size_ = 0;
  D3D12_RESOURCE_STATES scratch_buffer_state_;
  bool scratch_buffer_used_ = false;

  uint32_t current_queue_frame_ = UINT32_MAX;

  // The current fixed-function drawing state.
  D3D12_VIEWPORT ff_viewport_;
  D3D12_RECT ff_scissor_;
  float ff_blend_factor_[4];
  uint32_t ff_stencil_ref_;
  bool ff_viewport_update_needed_;
  bool ff_scissor_update_needed_;
  bool ff_blend_factor_update_needed_;
  bool ff_stencil_ref_update_needed_;

  // Currently bound graphics or compute pipeline.
  ID3D12PipelineState* current_pipeline_;
  // Currently bound graphics root signature.
  ID3D12RootSignature* current_graphics_root_signature_;
  // Extra parameters which may or may not be present.
  RootExtraParameterIndices current_graphics_root_extras_;
  // Whether root parameters are up to date - reset if a new signature is bound.
  uint32_t current_graphics_root_up_to_date_;

  // Currently bound descriptor heaps - update by RequestViewDescriptors and
  // RequestSamplerDescriptors.
  ID3D12DescriptorHeap* current_view_heap_;
  ID3D12DescriptorHeap* current_sampler_heap_;

  // System shader constants.
  HlslShaderTranslator::SystemConstants system_constants_;

  // Constant buffer bindings.
  struct ConstantBufferBinding {
    D3D12_GPU_VIRTUAL_ADDRESS buffer_address;
    bool up_to_date;
  };
  ConstantBufferBinding cbuffer_bindings_system_;
  ConstantBufferBinding cbuffer_bindings_float_[16];
  ConstantBufferBinding cbuffer_bindings_bool_loop_;
  ConstantBufferBinding cbuffer_bindings_fetch_;

  // Pages with the descriptors currently used for handling Xenos draw calls.
  uint64_t draw_view_full_update_;
  uint64_t draw_sampler_full_update_;

  // Latest descriptor handles used for handling Xenos draw calls.
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_common_constants_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_fetch_constants_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_vertex_float_constants_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_pixel_float_constants_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_shared_memory_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_pixel_textures_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_pixel_samplers_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_vertex_textures_;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_vertex_samplers_;

  // Current primitive topology.
  D3D_PRIMITIVE_TOPOLOGY primitive_topology_;
};

}  // namespace d3d12
}  // namespace gpu
}  // namespace xe

#endif  // XENIA_GPU_D3D12_D3D12_COMMAND_PROCESSOR_H_
