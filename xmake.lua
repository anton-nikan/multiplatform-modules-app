add_rules("mode.debug", "mode.release")
if is_mode("debug") or is_mode("check") or is_mode("profile") then
	add_defines("_DEBUG")
else
	add_defines("NDEBUG")
end

set_languages("c++23")
add_cxxflags("-fno-rtti")
-- add_requires("metal-cpp")

target("multiplatform-modules-app")

set_kind("binary")
set_policy("build.c++.modules", true)
add_includedirs("src")
add_files(
	"src/main.cpp",
	"src/native_storage.cpp",
	"src/platform.cpp",
	"src/application.cpp",
	"src/render.cpp",
	"src/resources.cpp"
)

if is_plat("macosx") then
	add_includedirs("lib/metal-cpp-extensions", "lib/metal-cpp")
	add_files("src/macos/*.cpp")
end

-- add_packages("metal-cpp")
add_frameworks("Metal", "Foundation", "Cocoa", "CoreGraphics", "MetalKit")
