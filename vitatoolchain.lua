-- vitatoolchain.lua
toolchain("vitasdk-clang")
    set_kind("standalone")

    local sdk = os.getenv("VITASDK")
    set_sdkdir(sdk)
    set_bindir(path.join(sdk, "bin"))

    -- Compiler
    set_toolset("cc", "clang-20")
    set_toolset("cxx", "clang++-20")
	add_defines("__vita__")

    -- Linker
    set_toolset("ld", "arm-vita-eabi-g++")
    -- OR: set_toolset("ld", "clang++")

    set_toolset("ar", "llvm-ar")
    set_toolset("strip", "llvm-strip")

    on_load(function (toolchain)
        local sysroot = path.join(sdk, "arm-vita-eabi")

        -- Target CPU / FPU
        toolchain:add("cxflags", "--target=armv7a-none-eabi")
        toolchain:add("cxflags", "-march=armv7-a")
        toolchain:add("cxflags", "-mfpu=neon")
        toolchain:add("cxflags", "-mfloat-abi=softfp")
        toolchain:add("cxflags", "--sysroot=" .. sysroot)

        -- Linker flags
        toolchain:add("ldflags", "--sysroot=" .. sysroot)
        toolchain:add("ldflags", "-Wl,-q")

        -- toolchain:add("cxflags", "-fno-builtin")
    end)
toolchain_end()
