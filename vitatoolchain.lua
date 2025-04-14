-- define toolchain
toolchain("vita")
	
	set_description("ps vita target with VITA SDK")

	--set_kind("cross")
    set_kind("standalone")
    set_sdkdir("/usr/local/vitasdk")
	set_bindir("/usr/local/vitasdk/bin")
	add_defines("__vita__")
	set_toolset("cxx", "arm-vita-eabi-g++")

    on_load(function (toolchain)
		-- add flags for arch
        toolchain:add("cxflags", "-march=armv7-a", "-msoft-float", {force = true})
        toolchain:add("cxxflags", "-march=armv7-a", "-msoft-float", {force = true})
        toolchain:add("ldflags", "-march=armv7-a", "-msoft-float", {force = true})
--        toolchain:add("ldflags", "--static", {force = true})
--        toolchain:add("syslinks", "gcc", "c")
    end)

toolchain_end()
