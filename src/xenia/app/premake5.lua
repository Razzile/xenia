project_root = "../../.."
include(project_root.."/tools/build")
local qt = premake.extensions.qt

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
    "xenia-ui-qt",
    "xenia-ui-spirv",
    "xenia-ui-vk",
    "xenia-ui-vulkan",
    "xenia-vfs",
    "xxhash",
  })

  -- Setup Qt libraries
  qt.enable()
  qtmodules{"core", "gui", "widgets"}
  qtprefix "Qt5"
  configuration {"Checked"}
  qtsuffix "d"
  configuration {"Debug"}
  qtsuffix "d"
  configuration {}
  if qt.defaultpath ~= nil then
    qtpath(qt.defaultpath)
  end

  -- Qt static configuration (if necessary). Used by AppVeyor.
  if os.getenv("QT_STATIC") then
    qt.modules["AccessibilitySupport"] = {
      name = "AccessibilitySupport",
      include = "QtAccessibilitySupport",
    }
    qt.modules["EventDispatcherSupport"] = {
      name = "EventDispatcherSupport",
      include = "QtEventDispatcherSupport",
    }
    qt.modules["FontDatabaseSupport"] = {
      name = "FontDatabaseSupport",
      include = "QtFontDatabaseSupport",
    }
    qt.modules["ThemeSupport"] = {
      name = "ThemeSupport",
      include = "QtThemeSupport",
    }
    qt.modules["VulkanSupport"] = {
      name = "VulkanSupport",
      include = "QtVulkanSupport",
    }

    defines({"QT_STATIC=1"})

    configuration {"not Checked"}
      links({
        "qtmain",
        "qtfreetype",
        "qtlibpng",
        "qtpcre2",
        "qtharfbuzz",
      })
    configuration {"Checked"}
      links({
        "qtmaind",
        "qtfreetyped",
        "qtlibpngd",
        "qtpcre2d",
        "qtharfbuzzd",
      })
    configuration {}
    qtmodules{"AccessibilitySupport", "EventDispatcherSupport", "FontDatabaseSupport", "ThemeSupport", "VulkanSupport"}
    libdirs("%{cfg.qtpath}/plugins/platforms")

    filter("platforms:Windows")
      -- Qt dependencies
      links({
        "dwmapi",
        "version",
        "imm32",
        "winmm",
        "netapi32",
        "userenv",
      })
      configuration {"not Checked"}
        links({"qwindows"})
      configuration {"Checked"}
        links({"qwindowsd"})
      configuration {}
    filter()
  end

 filter("platforms:Windows")
  entrypoint("mainCRTStartup")

  defines({
    "XBYAK_NO_OP_NAMES",
    "XBYAK_ENABLE_OMITTED_OPERAND",
    "XE_COMPILE_QT"
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
