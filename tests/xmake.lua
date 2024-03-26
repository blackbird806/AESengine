

add_defines("AES_UNITTESTS")
for _, filedir in ipairs(os.dirs("*")) do
	target(filedir)
		set_kind(binary)
		add_deps(wobEngine)
		add_files(filedir .. "/*.cpp")
end
