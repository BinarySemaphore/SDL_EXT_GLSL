@echo off
setlocal

echo Package Build for Distribution

:: Get dist info
set /p platform="Platform (windows | linux): "
if not "%platform%"=="windows" if not "%platform%"=="w" if not "%platform%"=="linux" if not "%platform%"=="l" (
	echo Unrecognized plaform: %platform%
	exit /b 1
)

set /p arch="Architecture (x86 | x64 | arm32 | arm64): "
if not "%arch%"=="x86" if not "%arch%"=="x64" if not "%arch%"=="arm32" if not "%arch%"=="arm64" (
	echo Unrcognized architecture: %arch%
	exit /b 1
)

:: Find lib file and config dist output name
echo Finding Built Library...
set build_dir=..\build
set lib_name=SDL_EXT_GLSL
set lib_final_name=SDL_EXT_GLSL

if "%arch%"=="x86" (
	set lib_final_name=%lib_final_name%_32
)
if "%arch%"=="arm32" (
	set lib_final_name=%lib_final_name%_ARM32
)
if "%arch%"=="arm64" (
	set lib_final_name=%lib_final_name%_ARM
)

for %%p in (windows w) do (
	if "%%p"=="%platform%" (
		set lib_name=%lib_name%.lib
		set lib_final_name=%lib_final_name%.lib
	)
)
for %%p in (linux l) do (
	if "%%p"=="%platform%" (
		set lib_name=lib%lib_name%.so
		set lib_final_name=lib%lib_final_name%.so
	)
)

set libpath=%build_dir%\%lib_name%
if not exist %libpath% (
	echo Could not find built library: %libpath%
	exit /b 1
)

echo Found %libpath%
echo:

:: Start assembling dist folder
echo Setup dist...
if not exist ..\dist (
	echo Create dist
	mkdir ..\dist
)
if not exist ..\dist\SDL_EXT_GLSL (
	echo Create dist\SDL_EXT_GLSL
	mkdir ..\dist\SDL_EXT_GLSL
	echo next
) else (
	echo Dist already exists
)
if not exist ..\dist\SDL_EXT_GLSL\include (
	echo Adding include headers
	mkdir ..\dist\SDL_EXT_GLSL\include
	copy ..\src\*.h ..\dist\SDL_EXT_GLSL\include\
) else (
	echo Dist already has includes
	set /p update="Update includes? (y/n): "
	if "%update%"=="y" (
		echo Updated includes
		copy ..\src\*.h ..\dist\SDL_EXT_GLSL\include\
	)
)
if not exist ..\dist\SDL_EXT_GLSL\%lib_final_name% (
	echo Adding static library for %platform% %arch% as %lib_final_name%
	copy %libpath% ..\dist\SDL_EXT_GLSL\%lib_final_name%
) else (
	echo Dist already has %lib_final_name% for %platform% %arch%
	set /p update="Update lib? (y/n): "
	if "%update%"=="y" (
		echo Updated lib %lib_final_name%
		copy %libpath% ..\dist\SDL_EXT_GLSL\%lib_final_name%
	)
)

echo Done
endlocal