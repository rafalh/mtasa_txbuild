project "txbuild"
	kind "ConsoleApp"
	language "C++"
	files {"**.h", "**.cpp"}
	includedirs "../tinyxml"
	links "tinyxml"

