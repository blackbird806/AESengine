set_project("WorldOfBirds")
set_license("GPLv3")

set_allowedmodes("debug", "release")
set_defaultmode("debug")
set_allowedplats("windows", "vita")

-- TODO rename engine
local engine_path = "AESengine"


set_languages("c99", "cxxlatest")

-- glm to remove
add_defines("GLM_FORCE_LEFT_HANDED","GLM_FORCE_DEPTH_ZERO_TO_ONE","GLM_FORCE_CTOR_INIT")

if is_mode("debug") then
	add_defines("_DEBUG", "AES_DEBUG")
elseif is_mode("release") then
	add_defines("NDEBUG", "AES_RELEASE")
end

if is_os("windows") then
	if is_mode("debug") then
		set_runtimes("MDd")
	elseif is_mode("release") then
		set_runtimes("MD")
	end
end

target("wobEngine")
	set_kind("static")
	add_includedirs(engine_path .. "/src", {public = true})
	add_includedirs(engine_path .. "/thirdParty", {public = true})
	add_includedirs(engine_path .. "/thirdParty/fmt/include", {public = true})
	add_includedirs(engine_path .. "/thirdParty/glm", {public = true})
	add_files(engine_path .. "/src/**.cpp|renderer/RHI/D3D11/*.cpp|renderer/RHI/SceGxm/*.cpp")
	add_headerfiles(engine_path .. "/src/**.hpp|renderer/RHI/D3D11/*.hpp|renderer/RHI/SceGxm/*.hpp")
	if is_os("windows") then
		add_files(engine_path .. "/src/renderer/RHI/D3D11/*.cpp")
		add_headerfiles(engine_path .. "/src/renderer/RHI/D3D11/*.hpp")
		add_defines("UNICODE", "_UNICODE", "AES_GRAPHIC_API_D3D11")
		add_links("d3d11", "dxgi", "d3dcompiler", "dxguid", "Dwmapi")

	elseif is_os("vita") then
		add_files(engine_path .. "/src/renderer/RHI/SceGxm/*.cpp")
		add_headerfiles(engine_path .. "/src/renderer/RHI/SceGxm/*.hpp")
		add_defines("AES_GRAPHIC_API_GXM")
	end


includes("test/xmake.lua")
includes("wobGame/xmake.lua")
