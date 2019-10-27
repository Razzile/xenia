project_root = "../../.."
include(project_root.."/tools/build")

group("src")
project("xenia-app")
  uuid("d7e98620-d007-4ad8-9dbd-b47c8853a17f")
  kind("WindowedApp")
  targetname("xenia")
  language("C++")
  links({
    "aes_128",
    "capstone",
    "discord-rpc",
    "glslang-spirv",
    "imgui",
    "libavcodec",
    "libavutil",
    "mspack",
    "snappy",
    "spirv-tools",
    "volk",
    "xenia-app-discord",
    "xenia-apu",
    "xenia-apu-nop",
    "xenia-apu-sdl",
    "xenia-base",
    "xenia-core",
    "xenia-cpu",
    "xenia-cpu-backend-x64",
    "xenia-debug-ui",
    "xenia-gpu",
    "xenia-gpu-null",
    "xenia-gpu-vk",
    "xenia-gpu-vulkan",
    "xenia-hid",
    "xenia-hid-nop",
    "xenia-hid-sdl",
    "xenia-kernel",
    "xenia-ui",
    "xenia-ui-spirv",
    "xenia-ui-vk",
    "xenia-ui-vulkan",
    "xenia-vfs",
    "xxhash",
  })
  defines({
    "XBYAK_NO_OP_NAMES",
    "XBYAK_ENABLE_OMITTED_OPERAND",
  })
  local_platform_files()
  files({
    "xenia_main.cc",
    "../base/main_"..platform_suffix..".cc",
    "../base/main_init_"..platform_suffix..".cc",
  })

  resincludedirs({
    project_root,
  })

  filter("platforms:Windows")
    files({
      "main_resources.rc",
    })

  filter("files:../base/main_init_"..platform_suffix..".cc")
    vectorextensions("IA32")  -- Disable AVX for main_init_win.cc so our AVX check doesn't use AVX instructions.

  filter("platforms:Linux")
    links({
      "X11",
      "xcb",
      "X11-xcb",
      "vulkan",
      "SDL2",
    })

  filter("platforms:Windows")
    links({
      "delayimp", -- This library implements delayed loading on Windows, an MSVC exclusive feature.
      "xenia-apu-xaudio2",
      "xenia-hid-winkey",
      "xenia-hid-xinput",
    })

  filter("platforms:Windows")
    linkoptions({
      "/DELAYLOAD:SDL2.dll",  -- SDL is not mandatory on Windows, implementations using native APIs are prefered.
    })

  filter("platforms:Windows")
    -- Only create the .user file if it doesn't already exist.
    local user_file = project_root.."/build/xenia-app.vcxproj.user"
    if not os.isfile(user_file) then
      debugdir(project_root)
      debugargs({
      })
    end
