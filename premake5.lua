workspace "LilRizer"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "LilRizer"
    location "LilRizer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-interm/" .. outputdir .. "%{prj.name}")
    
	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

    filter { "system:windows" }
        systemversion "latest"

    filter { "configurations: Debug" }
        runtime "Debug"
        symbols "On"

    filter { "configurations: Release" }
        runtime "Release"
        optimize "On"