add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

if is_plat("windows") then
    add_requires("detours v4.0.1-xmake.1")
elseif is_plat("android") then   

end

add_requires("nlohmann_json")

target("BetterRenderDragon")
    set_kind("shared")
    set_strip("all")
    set_languages("c++20")
    set_exceptions("none")
    add_headerfiles("src/(**.h)")
    add_includedirs("./src")
    add_includedirs("./include")
    add_defines("UNICODE")
    add_files("src/**.cpp")
    add_packages("nlohmann_json")

    if is_plat("windows") then
        add_packages("detours")
        remove_files("src/api/memory/android/**.cpp","src/api/memory/android/**.h")
        add_cxflags("/utf-8", "/EHa")
        add_links("runtimeobject","dxgi","ws2_32","ntdll","userenv")
    elseif is_plat("android") then
        remove_files(
            "src/api/memory/win/**.cpp",
            "src/api/memory/win/**.h",
            "src/dllmain.cpp",
            "src/MCPatches.cpp",
            "src/MCPatches.h"
            )
        add_linkdirs("lib/ARM64")
        add_links("GlossHook")
        add_cxflags("-O3")
    end