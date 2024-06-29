

add_defines("AES_UNITTESTS")
for _, filedir in ipairs(os.dirs("*")) do
	target(filedir)
		set_kind(binary)
		add_deps("wobEngine")
		add_files(filedir .. "/*.cpp")
		if is_os("windows") then
			add_links("User32") -- wtf idk why I need this
		end

end
