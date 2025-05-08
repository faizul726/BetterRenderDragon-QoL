add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

if is_plat("windows") then
    add_requires("detours v4.0.1-xmake.1")
    add_requires("imgui v1.91.0-docking", {configs = {dx11 = true, dx12 = true}})
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
        add_packages("detours","imgui")
        remove_files("src/api/memory/android/**.cpp","src/api/memory/android/**.h")
        add_cxflags("/utf-8", "/EHa")
        add_links("runtimeobject","dxgi","ws2_32","ntdll","userenv")
    end