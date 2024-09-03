includes("vitatoolchain.lua")

set_project("WorldOfBirds")
set_license("GPLv3")

set_allowedmodes("debug", "release")
set_defaultmode("debug")
--set_allowedplats("windows", "vita")

set_languages("c99", "cxxlatest")
add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")

-- glm to remove

add_defines("GLM_FORCE_LEFT_HANDED","GLM_FORCE_DEPTH_ZERO_TO_ONE","GLM_FORCE_CTOR_INIT")

if is_plat("vita") then
	set_toolchains("vita")
end

if is_mode("debug") then
	add_defines("_DEBUG", "AES_DEBUG")
	set_symbols("debug")
	set_optimize("none")
	set_warnings("all")
elseif is_mode("release") then
	add_defines("NDEBUG", "AES_RELEASE")
--	set_optimize("faster")
end

if is_os("windows") then
	if is_mode("debug") then
		set_runtimes("MDd")
	elseif is_mode("release") then
		set_runtimes("MD")
	end
end

add_includedirs("wobEngine/src")
add_includedirs("wobEngine/thirdParty")

target("wobEngine")
	set_kind("static")
	-- will be removed
	add_includedirs("wobEngine/thirdParty/fmt/include")
	add_includedirs("wobEngine/thirdParty/glm")
	--
	add_files("wobEngine/thirdParty/**.cpp|wobEngine/thirdParty/**.c")
	add_headerfiles("wobEngine/thirdParty/**.hpp|wobEngine/thirdParty/**.h")
	add_files("wobEngine/src/**.cpp|renderer/RHI/D3D11/*.cpp|renderer/RHI/SceGxm/*.cpp|core/platformWindows/*.cpp")
	add_headerfiles("wobEngine/src/**.hpp|renderer/RHI/D3D11/*.hpp|renderer/RHI/SceGxm/*.hpp|core/platformWindows/*.hpp")
	if is_os("windows") then
		add_files("wobEngine/src/renderer/RHI/D3D11/*.cpp")
		add_files("wobEngine/src/core/platformWindows/*.cpp")
		add_headerfiles("wobEngine/src/renderer/RHI/D3D11/*.hpp")
		add_headerfiles("wobEngine/src/core/platformWindows/*.hpp")
		add_defines("UNICODE", "_UNICODE", "AES_GRAPHIC_API_D3D11")
		add_links("d3d11", "dxgi", "d3dcompiler", "dxguid", "Dwmapi")
	elseif is_os("vita") then
		add_files("wobEngine/src/renderer/RHI/SceGxm/*.cpp")
		add_headerfiles("wobEngine/src/renderer/RHI/SceGxm/*.hpp")
		add_defines("AES_GRAPHIC_API_GXM")
	end

includes("tests/xmake.lua")
includes("wobGame/xmake.lua")
