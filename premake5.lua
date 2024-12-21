workspace "raycaster"
configurations { "Debug", "Release"}

project "raycaster"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    cppdialect "C++20"

    files { "src/*.cpp" }
    links { "SDL2" }
    libdirs { os.findlib("SDL2") }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"