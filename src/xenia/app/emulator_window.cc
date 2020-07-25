/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2018 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/app/emulator_window.h"
#include "xenia/apu/xaudio2/xaudio2_audio_system.h"
#include "xenia/base/cvar.h"
#include "xenia/gpu/vulkan/vulkan_graphics_system.h"
#include "xenia/ui/vulkan/vulkan_instance.h"
#include "xenia/ui/vulkan/vulkan_provider.h"

#include <QVulkanWindow>

#include "third_party/imgui/imgui.h"
#include "xenia/base/clock.h"
#include "xenia/base/cvar.h"
#include "xenia/base/debugging.h"
#include "xenia/base/logging.h"
#include "xenia/base/platform.h"
#include "xenia/base/profiling.h"
#include "xenia/base/threading.h"
#include "xenia/emulator.h"
#include "xenia/gpu/graphics_system.h"

DEFINE_string(apu, "any", "Audio system. Use: [any, nop, xaudio2]", "General");
DEFINE_string(gpu, "any", "Graphics system. Use: [any, vulkan, null]",
              "General");
DEFINE_string(hid, "any", "Input system. Use: [any, nop, winkey, xinput]",
              "General");

DEFINE_string(target, "", "Specifies the target .xex or .iso to execute.",
              "General");
DEFINE_bool(fullscreen, false, "Toggles fullscreen", "General");

DECLARE_bool(debug);

namespace xe {
namespace app {

class VulkanWindow : public QVulkanWindow {
 public:
  VulkanWindow(gpu::vulkan::VulkanGraphicsSystem* gfx)
      : graphics_system_(gfx) {}
  QVulkanWindowRenderer* createRenderer() override;

 private:
  gpu::vulkan::VulkanGraphicsSystem* graphics_system_;
};

class VulkanRenderer : public QVulkanWindowRenderer {
 public:
  VulkanRenderer(VulkanWindow* window,
                 gpu::vulkan::VulkanGraphicsSystem* graphics_system)
      : window_(window), graphics_system_(graphics_system) {}

  void startNextFrame() override {
    // Copy the graphics frontbuffer to our backbuffer.
    auto swap_state = graphics_system_->swap_state();

    auto cmd = window_->currentCommandBuffer();
    auto src = reinterpret_cast<VkImage>(
        swap_state->buffer_textures[swap_state->current_buffer]);
    auto dest = window_->swapChainImage(window_->currentSwapChainImageIndex());
    auto dest_size = window_->swapChainImageSize();

    VkImageMemoryBarrier barrier;
    std::memset(&barrier, 0, sizeof(VkImageMemoryBarrier));
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = src;
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);

    VkImageBlit region;
    region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.srcOffsets[0] = {0, 0, 0};
    region.srcOffsets[1] = {static_cast<int32_t>(swap_state->width),
                            static_cast<int32_t>(swap_state->height), 1};

    region.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.dstOffsets[0] = {0, 0, 0};
    region.dstOffsets[1] = {static_cast<int32_t>(dest_size.width()),
                            static_cast<int32_t>(dest_size.height()), 1};
    vkCmdBlitImage(cmd, src, VK_IMAGE_LAYOUT_GENERAL, dest,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region,
                   VK_FILTER_LINEAR);

    swap_state->pending = false;
    window_->frameReady();
  }

 private:
  gpu::vulkan::VulkanGraphicsSystem* graphics_system_;
  VulkanWindow* window_;
};

QVulkanWindowRenderer* VulkanWindow::createRenderer() {
  return new VulkanRenderer(this, graphics_system_);
}

EmulatorWindow::EmulatorWindow() {
  // TODO(DrChat): Pass in command line arguments.
  emulator_ = std::make_unique<xe::Emulator>(L"", L"");

  auto audio_factory = [&](cpu::Processor* processor,
                           kernel::KernelState* kernel_state) {
    auto audio = apu::xaudio2::XAudio2AudioSystem::Create(processor);
    if (audio->Setup(kernel_state) != X_STATUS_SUCCESS) {
      audio->Shutdown();
      return std::unique_ptr<apu::AudioSystem>(nullptr);
    }

bool EmulatorWindow::Initialize() {
  if (!window_->Initialize()) {
    XELOGE("Failed to initialize platform window");
    return false;
  }

  UpdateTitle();

  window_->on_closed.AddListener([this](UIEvent* e) { loop_->Quit(); });
  loop_->on_quit.AddListener([this](UIEvent* e) { window_.reset(); });

  window_->on_file_drop.AddListener(
      [this](FileDropEvent* e) { FileDrop(e->filename()); });

  window_->on_key_down.AddListener([this](KeyEvent* e) {
    bool handled = true;
    switch (e->key_code()) {
      case 0x4F: {  // o
        if (e->is_ctrl_pressed()) {
          FileOpen();
        }
      } break;
      case 0x6A: {  // numpad *
        CpuTimeScalarReset();
      } break;
      case 0x6D: {  // numpad minus
        CpuTimeScalarSetHalf();
      } break;
      case 0x6B: {  // numpad plus
        CpuTimeScalarSetDouble();
      } break;

      case 0x72: {  // F3
        Profiler::ToggleDisplay();
      } break;

      case 0x73: {  // VK_F4
        GpuTraceFrame();
      } break;
      case 0x74: {  // VK_F5
        GpuClearCaches();
      } break;
      case 0x76: {  // VK_F7
        // Save to file
        // TODO: Choose path based on user input, or from options
        // TODO: Spawn a new thread to do this.
        emulator()->SaveToFile(L"test.sav");
      } break;
      case 0x77: {  // VK_F8
        // Restore from file
        // TODO: Choose path from user
        // TODO: Spawn a new thread to do this.
        emulator()->RestoreFromFile(L"test.sav");
      } break;
      case 0x7A: {  // VK_F11
        ToggleFullscreen();
      } break;
      case 0x1B: {  // VK_ESCAPE
                    // Allow users to escape fullscreen (but not enter it).
        if (window_->is_fullscreen()) {
          window_->ToggleFullscreen(false);
        } else {
          handled = false;
        }
      } break;

      case 0x13: {  // VK_PAUSE
        CpuBreakIntoDebugger();
      } break;
      case 0x03: {  // VK_CANCEL
        CpuBreakIntoHostDebugger();
      } break;

      case 0x70: {  // VK_F1
        ShowHelpWebsite();
      } break;

      case 0x71: {  // VK_F2
        ShowCommitID();
      } break;

      default: { handled = false; } break;
    }
    e->set_handled(handled);
  });

  window_->on_mouse_move.AddListener([this](MouseEvent* e) {
    if (window_->is_fullscreen() && (e->dx() > 2 || e->dy() > 2)) {
      if (!window_->is_cursor_visible()) {
        window_->set_cursor_visible(true);
      }

      cursor_hide_time_ = Clock::QueryHostSystemTime() + 30000000;
    }

    return graphics;
  };

  window_->on_paint.AddListener([this](UIEvent* e) { CheckHideCursor(); });

  // Main menu.
  // FIXME: This code is really messy.
  auto main_menu = MenuItem::Create(MenuItem::Type::kNormal);
  auto file_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&File");
  {
    file_menu->AddChild(
        MenuItem::Create(MenuItem::Type::kString, L"&Open...", L"Ctrl+O",
                         std::bind(&EmulatorWindow::FileOpen, this)));
    file_menu->AddChild(
        MenuItem::Create(MenuItem::Type::kString, L"Close",
                         std::bind(&EmulatorWindow::FileClose, this)));
    file_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
    file_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"Show content directory...",
        std::bind(&EmulatorWindow::ShowContentDirectory, this)));
    file_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
    file_menu->AddChild(MenuItem::Create(MenuItem::Type::kString, L"E&xit",
                                         L"Alt+F4",
                                         [this]() { window_->Close(); }));
  }
  main_menu->AddChild(std::move(file_menu));

  // CPU menu.
  auto cpu_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&CPU");
  {
    cpu_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"&Reset Time Scalar", L"Numpad *",
        std::bind(&EmulatorWindow::CpuTimeScalarReset, this)));
    cpu_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"Time Scalar /= 2", L"Numpad -",
        std::bind(&EmulatorWindow::CpuTimeScalarSetHalf, this)));
    cpu_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"Time Scalar *= 2", L"Numpad +",
        std::bind(&EmulatorWindow::CpuTimeScalarSetDouble, this)));
  }
  cpu_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
  {
    cpu_menu->AddChild(MenuItem::Create(MenuItem::Type::kString,
                                        L"Toggle Profiler &Display", L"F3",
                                        []() { Profiler::ToggleDisplay(); }));
    cpu_menu->AddChild(MenuItem::Create(MenuItem::Type::kString,
                                        L"&Pause/Resume Profiler", L"`",
                                        []() { Profiler::TogglePause(); }));
  }
  cpu_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
  {
    cpu_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"&Break and Show Guest Debugger",
        L"Pause/Break",
        std::bind(&EmulatorWindow::CpuBreakIntoDebugger, this)));
    cpu_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"&Break into Host Debugger",
        L"Ctrl+Pause/Break",
        std::bind(&EmulatorWindow::CpuBreakIntoHostDebugger, this)));
  }
  main_menu->AddChild(std::move(cpu_menu));

  // GPU menu.
  auto gpu_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&GPU");
  {
    gpu_menu->AddChild(
        MenuItem::Create(MenuItem::Type::kString, L"&Trace Frame", L"F4",
                         std::bind(&EmulatorWindow::GpuTraceFrame, this)));
  }
  gpu_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
  {
    gpu_menu->AddChild(
        MenuItem::Create(MenuItem::Type::kString, L"&Clear Caches", L"F5",
                         std::bind(&EmulatorWindow::GpuClearCaches, this)));
  }
  main_menu->AddChild(std::move(gpu_menu));

  // Window menu.
  auto window_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&Window");
  {
    window_menu->AddChild(
        MenuItem::Create(MenuItem::Type::kString, L"&Fullscreen", L"F11",
                         std::bind(&EmulatorWindow::ToggleFullscreen, this)));
  }
  main_menu->AddChild(std::move(window_menu));

  // Help menu.
  auto help_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&Help");
  {
    help_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"Build commit on GitHub...", L"F2",
        std::bind(&EmulatorWindow::ShowCommitID, this)));
    help_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"Recent changes on GitHub...", [this]() {
          std::wstring url =
              std::wstring(L"https://github.com/xenia-project/xenia/compare/") +
              xe::to_wstring(XE_BUILD_COMMIT) + L"..." +
              xe::to_wstring(XE_BUILD_BRANCH);
          LaunchBrowser(url.c_str());
        }));
    help_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
    help_menu->AddChild(
        MenuItem::Create(MenuItem::Type::kString, L"&Website...", L"F1",
                         std::bind(&EmulatorWindow::ShowHelpWebsite, this)));
    help_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, L"&About...",
        [this]() { LaunchBrowser(L"https://xenia.jp/about/"); }));
  }
  main_menu->AddChild(std::move(help_menu));

  window_->set_main_menu(std::move(main_menu));

  window_->Resize(1280, 720);

  window_->DisableMainMenu();

  return true;
}

void EmulatorWindow::FileDrop(const wchar_t* filename) {
  auto result = emulator_->LaunchPath(filename);
  if (XFAILED(result)) {
    // TODO: Display a message box.
    XELOGE("Failed to launch target: %.8X", result);
  }
}

void EmulatorWindow::FileOpen() {
  std::wstring path;

  auto file_picker = xe::ui::FilePicker::Create();
  file_picker->set_mode(ui::FilePicker::Mode::kOpen);
  file_picker->set_type(ui::FilePicker::Type::kFile);
  file_picker->set_multi_selection(false);
  file_picker->set_title(L"Select Content Package");
  file_picker->set_extensions({
      {L"Supported Files", L"*.iso;*.xex;*.xcp;*.*"},
      {L"Disc Image (*.iso)", L"*.iso"},
      {L"Xbox Executable (*.xex)", L"*.xex"},
      //{ L"Content Package (*.xcp)", L"*.xcp" },
      {L"All Files (*.*)", L"*.*"},
  });
  if (file_picker->Show(window_->native_handle())) {
    auto selected_files = file_picker->selected_files();
    if (!selected_files.empty()) {
      path = selected_files[0];
    }
  }

  if (!path.empty()) {
    // Normalize the path and make absolute.
    std::wstring abs_path = xe::to_absolute_path(path);

    auto result = emulator_->LaunchPath(abs_path);
    if (XFAILED(result)) {
      // TODO: Display a message box.
      XELOGE("Failed to launch target: %.8X", result);
    }
  }
}

void EmulatorWindow::FileClose() {
  if (emulator_->is_title_open()) {
    emulator_->TerminateTitle();
  }
}

void EmulatorWindow::ShowContentDirectory() {
  std::wstring target_path;

  auto content_root = emulator_->content_root();
  if (!emulator_->is_title_open() || !emulator_->kernel_state()) {
    target_path = content_root;
  } else {
    // TODO(gibbed): expose this via ContentManager?
    wchar_t title_id[9] = L"00000000";
    std::swprintf(title_id, 9, L"%.8X", emulator_->kernel_state()->title_id());
    auto package_root = xe::join_paths(content_root, title_id);
    target_path = package_root;
  }

  if (!xe::filesystem::PathExists(target_path)) {
    xe::filesystem::CreateFolder(target_path);
  }

  LaunchBrowser(target_path.c_str());
}

void EmulatorWindow::CheckHideCursor() {
  if (!window_->is_fullscreen()) {
    // Only hide when fullscreen.
    return;
    return;
  }

  // Set a callback on launch
  emulator_->on_launch.AddListener([this]() {
    auto title_db = this->emulator()->game_data();
    if (title_db) {
      QPixmap p;
      auto icon_block = title_db->icon();
      if (icon_block.buffer &&
          p.loadFromData(icon_block.buffer, uint(icon_block.size), "PNG")) {
        this->setWindowIcon(QIcon(p));
      }
    }
  });
}

bool EmulatorWindow::InitializeVulkan() {
  auto provider =
      reinterpret_cast<ui::vulkan::VulkanProvider*>(graphics_provider_.get());

void EmulatorWindow::CpuBreakIntoDebugger() {
  if (!cvars::debug) {
    xe::ui::ImGuiDialog::ShowMessageBox(window_.get(), "Xenia Debugger",
                                        "Xenia must be launched with the "
                                        "--debug flag in order to enable "
                                        "debugging.");
    return;
  }
  auto processor = emulator()->processor();
  if (processor->execution_state() == cpu::ExecutionState::kRunning) {
    // Currently running, so interrupt (and show the debugger).
    processor->Pause();
  } else {
    // Not running, so just bring the debugger into focus.
    processor->ShowDebugger();
  }

void EmulatorWindow::CpuBreakIntoHostDebugger() { xe::debugging::Break(); }

void EmulatorWindow::GpuTraceFrame() {
  emulator()->graphics_system()->RequestFrameTrace();
}

void EmulatorWindow::GpuClearCaches() {
  emulator()->graphics_system()->ClearCaches();
}

  // Now set the graphics window as our central widget.
  QWidget* wrapper = QWidget::createWindowContainer(graphics_window_.get());
  setCentralWidget(wrapper);

  return true;
}

void EmulatorWindow::ShowHelpWebsite() { LaunchBrowser(L"https://xenia.jp"); }

void EmulatorWindow::ShowCommitID() {
  std::wstring url =
      std::wstring(L"https://github.com/xenia-project/xenia/commit/") +
      xe::to_wstring(XE_BUILD_COMMIT) + L"/";
  LaunchBrowser(url.c_str());
}

void EmulatorWindow::UpdateTitle() {
  std::wstring title(base_title_);

  if (emulator()->is_title_open()) {
    auto game_title = emulator()->game_title();
    title += xe::format_string(L" | [%.8X] %s", emulator()->title_id(),
                               game_title.c_str());
  }

  auto graphics_system = emulator()->graphics_system();
  if (graphics_system) {
    auto graphics_name = graphics_system->name();
    title += L" <" + graphics_name + L">";
  }

  if (Clock::guest_time_scalar() != 1.0) {
    title += xe::format_string(L" (@%.2fx)", Clock::guest_time_scalar());
  }

  window_->set_title(title);
}

}  // namespace app
}  // namespace xe
