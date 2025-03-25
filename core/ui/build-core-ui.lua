project "CharUI"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   staticruntime "off"

   files { "include/ui/*.h", "src/*.cpp" }

   includedirs
   {
      "include/ui",
      "../engine/include"
   }

   links {
       "ChessEngine"
   }

   targetdir ("bin/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")
   objdir ("obj/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

   filter "system:windows"
       systemversion "latest"
       staticruntime "on"

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"

project "CharUITest"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   staticruntime "off"

   files { "test/*.h", "test/*.cpp" }

   links
   {
      "CharUI",
      "GTest"
   }
   includedirs
   {
      "include/ui",
      "../engine/include",
      "%{wks.location}/vendor/gtest/include"

   }

   targetdir ("bin/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")
   objdir ("obj/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "off"
