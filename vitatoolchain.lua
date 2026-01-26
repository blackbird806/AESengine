-- define toolchain
toolchain("vita")
	
	set_description("ps vita target with VITA SDK")

	--set_kind("cross")
    set_kind("standalone")
--  set_sdkdir("/usr/local/vitasdk")
--	set_bindir("/usr/local/vitasdk/bin")
	add_defines("__vita__")

	use_vita_toolchain = false
--	if use_vita_toolchain then
--		set_toolset("cxx", "arm-vita-eabi-g++")
--		set_toolset("ld", "arm-vita-eabi-g++")
--		set_toolset("ar", "arm-vita-eabi-g++")
--		set_toolset("cc", "arm-vita-eabi-gcc")
--	else
		set_toolset("cxx", "clang++")
		set_toolset("ld", "clang++")
		set_toolset("ar", "clang+++")
		set_toolset("cc", "clang")
--	end

    on_load(function (toolchain)
		-- add flags for arch
		--https://github.com/psvsdk/psvsdk/blob/master/src/psv-gcc.sh
        toolchain:add("cxflags", "-march=armv7-a", "-mfloat-abi=hard", 
						"-mtune=cortex-a9", "-mfpu=neon", "-mthumb" , {force = true})
        toolchain:add("cxxflags", "-march=armv7-a", "-msoft-float", {force = true})
        toolchain:add("ldflags", "-march=armv7-a", "-msoft-float", {force = true})
--        toolchain:add("ldflags", "--static", {force = true})
--        toolchain:add("syslinks", "gcc", "c")
    end)

toolchain_end()
