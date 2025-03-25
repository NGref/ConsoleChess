-- premake5.lua
workspace "ConsoleChess"
   architecture "x64"
   configurations { "Debug", "Release"}
   startproject "App"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/MTd" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Vendor"
	include "vendor/gtest/build-gtest.lua"
group ""

group "Core"
	include "core/engine/build-core-engine.lua"
   include "core/ui/build-core-ui.lua"
group ""

include "app/build-app.lua"