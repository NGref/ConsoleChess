project "ChessEngine"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   staticruntime "off"

   files { "include/engine/*.h", "src/*.cpp" }

   includedirs
   {
      "include/engine"
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

project "ChessEngineTest"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   staticruntime "off"

   files { "test/*.h", "test/*.cpp" }

   links
   {
      "ChessEngine",
      "GTest"
   }
   includedirs
   {
      "include/engine",
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