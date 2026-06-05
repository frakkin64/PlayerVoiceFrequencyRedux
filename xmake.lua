-- include subprojects
includes("lib/commonlibf4")

-- set project constants
set_project("PlayerVoiceFrequencyRedux")
set_version("1.2.0")
set_license("GPL-3.0")
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set config
set_config("commonlib_ini", true)

-- define targets
target("PlayerVoiceFrequency")
    add_rules("commonlibf4.plugin", {
        name = "PlayerVoiceFrequencyRedux",
        author = "frakkin64",
        description = "F4SE plugin using CommonLibF4"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h", "include/**.h")
    add_includedirs("src", "include")
    set_pcxxheader("src/pch.h")
