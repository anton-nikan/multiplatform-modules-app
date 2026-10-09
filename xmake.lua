add_rules("mode.debug", "mode.release")
if is_mode("debug") or is_mode("check") or is_mode("profile") then
	add_defines("_DEBUG")
else
	add_defines("NDEBUG")
end

set_languages("c++23")
add_cxxflags("-fno-rtti")

-- Platform backend, selects the implementation units compiled for platform, application and render:
--   macos: src/macos (Cocoa + Metal via metal-cpp), default on macOS
--   sdl:   src/sdl (SDL3)
-- Switch with: xmake f --backend=sdl
option("backend")
	set_showmenu(true)
	set_default(is_host("macosx") and "macos" or "sdl")
	set_values("macos", "sdl")
	set_description("Platform backend to build")
option_end()

local backend = get_config("backend")

-- add_requires("metal-cpp")
add_requires("tinygltf v2.9.7")
if backend == "sdl" then
	add_requires("libsdl3")
end

target("multiplatform-modules-app")

on_load(function (target)
	local backend = get_config("backend")
	if backend == "macos" and not is_plat("macosx") then
		raise("backend 'macos' is only available when building for macOS, use --backend=sdl")
	elseif backend ~= "macos" and backend ~= "sdl" then
		raise("no platform backend selected, use --backend=macos or --backend=sdl")
	end
end)

set_kind("binary")
set_policy("build.c++.modules", true)
add_includedirs("src")
add_files(
	"src/main.cpp",
	"src/native_storage.cpp",
	"src/platform.cpp",
	"src/application.cpp",
	"src/render.cpp",
	"src/resources.cpp",
	"src/model.cpp"
)

if backend == "macos" then
	add_includedirs("lib/metal-cpp-extensions", "lib/metal-cpp")
	add_files("src/macos/*.cpp")
	add_frameworks("Metal", "Foundation", "Cocoa", "CoreGraphics", "MetalKit")
elseif backend == "sdl" then
	add_files("src/sdl/*.cpp")
	add_packages("libsdl3")
end

-- enabling gltf models
add_files("src/impl/model-gltf.cpp", "src/impl/tinygltf.cpp")
add_packages("tinygltf")

-- add_packages("metal-cpp")
