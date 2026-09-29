
 -- description de la base de code
dofile "gkit.lua"

 -- description des projets		 
projects = {
    "base",
    "tp",
    "compute",
}

for i, name in ipairs(projects) do
    project (name )
        language "C++"
        kind "ConsoleApp"
        targetdir "bin"
        links { "gkit" }
        includedirs { ".", "src/gKit" }
        files { "projets/" .. name .. ".cpp" }
end

 -- description des utilitaires
tools= {
    "shader_kit",
    "shader_kit_debug",
    "image_viewer"
}

for i, name in ipairs(tools) do
    project(name)
        language "C++"
        kind "ConsoleApp"
        targetdir "bin"
        links { "gkit" }
        includedirs { ".", "src/gKit" }
        files { "src/" .. name .. ".cpp" }
end

 -- et des tutos, s'ils sont présents...
if os.isfile("gkit2_tutos/premake5.lua") then
    include "gkit2_tutos"
end

 -- lifgraphique
if os.isfile("lifgraphique/premake5.lua") then
    include "lifgraphique"
end

