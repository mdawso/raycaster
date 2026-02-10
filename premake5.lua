workspace "raycaster"
configurations { "Debug", "Release"}

project "raycaster"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    cppdialect "C++23"

    files { "src/**.cpp", "src/**.hpp" }
    links { "SDL3" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"