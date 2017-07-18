(workspace or solution) "TxBuild"
	configurations { "Release" }
	language "C++"
	targetdir "../bin"
	objdir "../obj"
	if location then
		location "premake-output"
	end
	defines {
		"NOMINMAX",
	}
	configuration "vs*"
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE",
		}
	configuration "not vs*"
		buildoptions { "-std=c++11" }
		linkoptions {"-static-libgcc", "-static-libstdc++", "-static"}
	
	configuration "Debug"
		defines { "DEBUG" }
		if symbols then
			symbols "On"
		else
			flags { "Symbols" }
		end
	
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSize" }
	
	configuration "windows"
		defines { "WIN32", "_WINDOWS" }
	
	configuration "linux"
		defines { "LINUX" }
	
	include "tinyxml"
	include "addutf8bom"
	include "txbuild"
	include "txprotect"
	include "txstrlist"
