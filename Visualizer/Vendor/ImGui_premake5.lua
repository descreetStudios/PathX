project "ImGui"
	location "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "off"
	systemversion "latest"

	targetdir ("%{wks.location}/%{cfg.system}/%{cfg.buildcfg}/")
	objdir ("%{prj.location}/%{cfg.system}/%{cfg.buildcfg}/")

	files
	{
		"ImGui/imconfig.h",
		"ImGui/imgui.h",
		"ImGui/imgui.cpp",
		"ImGui/imgui_draw.cpp",
		"ImGui/imgui_internal.h",
		"ImGui/imgui_widgets.cpp",
		"ImGui/imstb_rectpack.h",
		"ImGui/imstb_textedit.h",
		"ImGui/imstb_truetype.h",
		"ImGui/imgui_demo.cpp",
		"ImGui/imgui_tables.cpp",

		-- API Specific
		"ImGui/backends/imgui_impl_glfw.cpp",
		"ImGui/backends/imgui_impl_glfw.h",
		"ImGui/backends/imgui_impl_opengl3.cpp",
		"ImGui/backends/imgui_impl_opengl3.h",
		"ImGui/backends/imgui_impl_opengl3_loader.h"
	}

	includedirs
	{
		"ImGui/",
		"GLFW/include/"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
