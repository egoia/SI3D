workspace "gKit"
    configurations { "debug", "release" }

    filter { "configurations:debug" }
        defines { "DEBUG" }
        symbols "on"
    
    filter { "configurations:release" }
--~ 		defines { "NDEBUG" }
--~ 		defines { "GK_RELEASE" }
        optimize "speed"
        
    filter { "system:linux" }
        cppdialect "c++14"
        buildoptions { "-mtune=native -march=native" }
        buildoptions { "-Wall -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
        
    filter { "system:linux" }
        links { "SDL2", "GL" }
    
    filter { "system:linux", "configurations:debug" }
        buildoptions { "-g"}
        linkoptions { "-g"}
--~         openmp "off"
    
    filter { "system:linux", "configurations:release" }
--~         openmp "on"
        buildoptions { "-flto"}
        linkoptions { "-flto=auto"}
       
    
    filter { "system:windows", "action:vs*" }
        location "build"
        debugdir "."
        
        system "Windows"
        architecture "x64"
        disablewarnings { "4244", "4305" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild" }
        
        defines { "WIN32", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
        defines { "NOMINMAX" } -- allow std::min() and std::max() in vc++ :(((
        
        cppdialect "c++14"
        openmp "on"
        vectorextensions "avx2"
        floatingpoint "strict"
        exceptionhandling "off"
        
        includedirs { "extern/visual/include" }
        libdirs { "extern/visual/lib" }
        links { "opengl32", "SDL2", "SDL2main" }
        
        -- beuk, mais necessaire...
        if _ACTION == "vs*" then
            printf("\ncopying dll...")
            if not os.isdir("extern/visual/bin/") then
                printf("[error] no dll...\n")
            end
            os.mkdir("bin")
            os.copyfile("extern/visual/bin/SDL2.dll", "bin/SDL2.dll")
        end
    
    filter { "system:windows", "action:codeblocks" }
        location "build"
        debugdir "."
        
        system "Windows"
        architecture "x64"
        cppdialect "c++14"
        openmp "on"
        vectorextensions "avx2"
        floatingpoint "strict"
        exceptionhandling "off"
        
        defines { "_USE_MATH_DEFINES" }
        
        includedirs { "extern/mingw/include" }
        libdirs { "extern/mingw/lib" }
        links { "mingw32", "opengl32", "SDL2main", "SDL2" }
        -- mingw32,  SDL2main, gkit, SDL2, opengl32
    
    filter { "system:macosx" }
        buildoptions { "-Wno-deprecated-declarations" }
        
        cppdialect "c++14"
        frameworks= "-F extern/macos"
        defines { "GK_MACOS" }
        buildoptions { frameworks }
        linkoptions { frameworks, "-framework OpenGL -framework SDL2 -rpath extern/macos" }
    
project "gkit"
    language "C++"
    kind "StaticLib"
    targetdir "bin"
    includedirs { ".", "src/gKit" }
    files { "src/gKit/*.cpp", "src/gKit/*.c",  "src/gKit/*.h" }
    
