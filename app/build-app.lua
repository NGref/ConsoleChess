project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "src",
      "../core/ui/include",
      "../core/engine/include"
   }

   links
   {
      "CharUI",
       "ChessEngine"
   }

   targetdir ("bin/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")
   objdir ("obj/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"