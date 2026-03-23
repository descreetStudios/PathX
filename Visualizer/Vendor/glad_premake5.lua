project "glad"
	location "glad"
	kind "StaticLib"
	language "C"
	staticruntime "on"
	systemversion "latest"

	targetdir ("%{wks.location}/%{cfg.system}/%{cfg.buildcfg}/")
	objdir ("%{prj.location}/%{cfg.system}/%{cfg.buildcfg}/")

	files
	{
		"glad/include/glad/glad.h",
		"glad/include/KHR/khrplatform.h",
		"glad/src/glad.c"
	}

	includedirs
	{
		"glad/include"
	}

	filter "system:linux"
		pic "on"

	filter "system:macosx"
		pic "on"

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