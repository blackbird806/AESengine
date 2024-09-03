-- define toolchain
toolchain("vita")
	
	set_description("ps vita target with VITA SDK")

    set_kind("cross")
    set_sdkdir("/usr/local/vitasdk")
	add_defines("__vita__")

    on_load(function (toolchain)
		-- add flags for arch
        toolchain:add("cxflags", "-march=armv7-a", "-msoft-float", {force = true})
        toolchain:add("cxxflags", "-march=armv7-a", "-msoft-float", {force = true})
        toolchain:add("ldflags", "-march=armv7-a", "-msoft-float", {force = true})
--        toolchain:add("ldflags", "--static", {force = true})
--        toolchain:add("syslinks", "gcc", "c")
    end)

toolchain_end()
