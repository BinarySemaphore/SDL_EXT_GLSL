#!/bin/bash

echo "Package Build for Distribution"

# Figure out where we're running
echo "Checking location..."
src_dir="./src"
build_dir="./build"
dist_dir="./dist"

if [ ! -d "$src_dir" ]; then
	src_dir=".$src_dir"
	build_dir=".$build_dir"
	dist_dir=".$dist_dir"
	if [ ! -d "$src_dir" ]; then
		echo "Cannot find source directory \"$src_dir\" from here or one directory above"
		echo "Make sure you're running this script from the project root or scripts folder"
		read -p "Press enter to continue..."
		exit
	else
		echo "Running from project scripts directory"
	fi
else
	echo "Running from project root"
fi

echo ""

# Get dist info
read -p "Platform (windows | linux): " platform
if [[ "$platform" != "windows" && "$platform" != "w" && "$platform" != "linux" && "$platform" != "l" ]]; then
	echo "Unrecognized platform: $platform"
	read -p "Press enter to continue..."
	exit
fi

read -p "Architecture (x86 | x64 | arm32 | arm64): " arch
if [[ "$arch" != "x86" && "$arch" != "x64" && "$arch" != "arm32" && "$arch" != "arm64" ]]; then
	echo "Unrecognized architecture: $arch"
	read -p "Press enter to continue..."
	exit
fi

# Find lib file and config dist output name
echo "Finding built library..."
lib_name="SDL_EXT_GLSL"
lib_final_name="SDL_EXT_GLSL"

if [ "$arch" == "x86" ]; then
	lib_final_name+="_32"
fi
if [ "$arch" == "arm32" ]; then
	lib_final_name+="_ARM32"
fi
if [ "$arch" == "arm64" ]; then
	lib_final_name+="_ARM"
fi

if [[ "$platform" == "windows" || "$platform" == "w" ]]; then
	lib_name+=".lib"
	lib_final_name+=".lib"
fi
if [[ "$platform" == "linux" || "$platform" == "l" ]]; then
	lib_name="lib$lib_name.so"
	lib_final_name="lib$lib_final_name.so"
fi

libpath="$build_dir/$lib_name"
if [ ! -f "$libpath" ]; then
	echo "Could not find built library: $libpath"
	read -p "Press enter to continue..."
	exit
fi

echo "Found $libpath"
echo ""

# Assemble dist directory
echo "Setup dist..."
if [ ! -d "$dist_dir" ]; then
	echo "Create dist"
	mkdir "$dist_dir"
fi
if [ ! -d "$dist_dir/SDL_EXT_GLSL" ]; then
	echo "Create dist/SDL_EXT_GLSL"
	mkdir "$dist_dir/SDL_EXT_GLSL"
else
	echo "Dist already exists"
fi
if [ ! -d "$dist_dir/SDL_EXT_GLSL/include" ]; then
	echo "Adding include headers"
	mkdir "$dist_dir/SDL_EXT_GLSL/include"
	cp $src_dir/*.h "$dist_dir/SDL_EXT_GLSL/include/"
else
	echo "Dist already has includes"
	read -p "Update includes? (y/n): " update
	if [ "$update" == "y" ]; then
		rm -rf "$dist_dir/SDL_EXT_GLSL/include/*"
		cp $src_dir/*.h "$dist_dir/SDL_EXT_GLSL/include/"
		echo "Updated includes"
	fi
fi
if [ ! -f "$dist_dir/SDL_EXT_GLSL/$lib_final_name" ]; then
	echo "Adding library for $platform $arch as $lib_final_name"
	cp "$libpath" "$dist_dir/SDL_EXT_GLSL/$lib_final_name"
else
	echo "Dist already has $lib_final_name for $platfrom $arch"
	read -p "Update lib? (y/n): " update
	if [ "$update" == "y" ]; then
		cp "$libpath" "$dist_dir/SDL_EXT_GLSL/$lib_final_name"
		echo "Updated lib $lib_final_name"
	fi
fi

echo "Done"
read -p "Press enter to continue..."
exit
