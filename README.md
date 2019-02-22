# Jessene
A lua module for the Godot engine written in GDNative.

NOTE: This repository only contains the source code for this module and does not include the lua source code or godot-cpp files.
Download lua 5.3.5 from: [here](https://www.lua.org/ftp/).
Download the godot-cpp files from: [here](https://github.com/GodotNativeTools/godot-cpp).

## Compiling Instructions:
### Lua
* Move lua's `src` directory and its `Makefile` into the same directory as this module.
* Compile lua.
    * On Linux run: `make linux test`
    * On macOS run: `make macosx test`
    * On Windows:
        * Download the Visual C compiler.
        * Run the commands:
            ```
            cl /MD /O2 /c /DLUA_BUILD_AS_DLL *.c
            ren lua.obj lua.o
            ren luac.obj luac.o
            link /DLL /IMPLIB:liblua.lib /OUT:liblua.dll *.obj 
            ```
* Rename lua's `src` directory to `luasrc`.  At this point the folder structure should look like this:
    ```
    jessene/
    ├─luasrc/
    | └─lua source code files
    └─jessene files
    ```
### godot-cpp
* Place the godot-cpp repository in the same directory containing the jessene repository.
* Follow all instructions for compiling godot-cpp in the readme on that repo (you can skip the api.json).
* Afterwards, the folder structure should look like this:
    ```
    Godot-Lua-Library/
    ├─godot-cpp/
    | └─godot_headers/
    ├─bin/
    └─src/
      └─jessene/
        ├─luasrc/
        | └─lua source code files
        └─jessene files
    ```
### Jessene
* In `Godot-Lua-Library/src/jessene/` there is an `SConstruct` file.  Move the `SConstruct` file to `Godot-Lua-Library/`.
* Run: `scons platform=(linux|windows|osx)` from `Godot-Lua-Library/`.

And now you are done. Attached in the repo is also a copy of a gdns and gdnlib file to import the lua module into godot. 
When importating the module, make sure that `liblua.a` for Linux/macOS or `liblua.dll` for Windows is in the same folder as the compiled LuaScript module.

## Using in Godot
In order to use this module in Godot, attach the gdns script to a node (we will call this LuaNode).

Let's say you have a simple lua script under `res://lua_scripts/utils.lua`
```lua
function sum(...)
    result = 0
    local arg = {...}
    for i,v in ipairs(arg) do
       result = result + v
    end
    return result
end
```
To use the `sum(...)` function in Godot, do:
```
onready var Lua = $"LuaNode"

func foo():
    Lua.load("res://lua_scripts/utils.lua")
    var sum = Lua.execute("sum", [1, 2, 3, 4])
```
`sum` will now equal 10.
