workspace "PathX"
    architecture "x64"
    startproject "Visualizer"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    IncludeDirs = {}
	IncludeDirs["spdlog"] = "Engine/Vendor/spdlog/include"

project "Engine"
    location "Engine"
    kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
    systemversion "latest"

    targetdir ("%{wks.location}/%{cfg.system}/%{cfg.buildcfg}/")
	objdir ("%{prj.location}/%{cfg.system}/%{cfg.buildcfg}/")

    fatalwarnings { "All" }
	warnings ("Extra")
	buildoptions "/utf-8"

    files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
	}

    includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/Source/Common",
		"%{IncludeDirs.spdlog}",
    }

    filter "configurations:Debug"
		defines "PATHX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PATHX_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"

    filter "configurations:Dist"
		defines "PATHX_DIST"
		runtime "Release"
		optimize "on"

project "Visualizer"
	location "Visualizer"
    kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
    systemversion "latest"

	targetdir ("%{wks.location}/%{cfg.system}/%{cfg.buildcfg}/")
	objdir ("%{prj.location}/%{cfg.system}/%{cfg.buildcfg}/")

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"Engine/Source",
		"Engine/Source/Common",
		"%{IncludeDirs.spdlog}",
	}

	links
	{
		"Engine"
	}
	
filter "configurations:Debug"
		defines "PATHX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PATHX_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Dist"
		defines "PATHX_DIST"
		runtime "Release"
		optimize "on"

project "EngineTest"
    location "EngineTest"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
	staticruntime "on"
    systemversion "latest"

    targetdir ("%{wks.location}/%{cfg.system}/%{cfg.buildcfg}/")
	objdir ("%{prj.location}/%{cfg.system}/%{cfg.buildcfg}/")

	buildoptions "/utf-8"

    files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

    includedirs
	{
		"%{prj.name}/Source",
		"Engine/Source",
		"Engine/Source/Common",
		"%{IncludeDirs.spdlog}",
	}

    links
	{
		"Engine"
	}

    filter "configurations:Debug"
		defines "PATHX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PATHX_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Dist"
		defines "PATHX_DIST"
		runtime "Release"
		optimize "on"