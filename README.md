# godot-lua-module
a lua module for godot written in GDNative.

this is only the source code for the module and does not include the lua source code or godot-cpp files.

## Compiling Instructions:

first off you will need the lua source code. this can be downloaded from here http://www.lua.org/ftp/ .
You only need the folder named "src" inside the download. Extract this and place it in the same directory as this module.

you then need to compile lua, this can be done by running `make linux test` on linux or `make macosx test` on mac

for windows you will need visual c++ compiler and run this series of commands
```
cl /MD /O2 /c /DLUA_BUILD_AS_DLL *.c
ren lua.obj lua.o
ren luac.obj luac.o
link /DLL /IMPLIB:liblua.lib /OUT:liblua.dll *.obj 
```

the module and the rest of this instruction assumes that the folder containg all of the source code is named "luasrc"

at this point the folder structure should look like this
```
godot-lua-module/
├─luasrc/
| └─lua src files
└─godot-lua-module files
```

Next you need the godot-cpp repo. This can be aquired from https://github.com/GodotNativeTools/godot-cpp .
This repository will go outside the folder for this modules source code
Follow all instructions for compiling godot-cpp in the readme on that repo (you can skip the api.json).

in the end the folder structure should look like this
```
Godot-Lua-Library/
├─godot-cpp/
| └─godot_headers/
├─bin/
└─src/
  └─godot-lua-module/
    ├─luasrc/
    | └─lua src files
    └─godot-lua-module files
```

in this repository is a scons file (this is diffrent from the one for compiling godot-cpp).
At this point move the scons file to the root directory (ie Godot-Lua-Library) and then run `scons platform=(linux|windows|osx)`


and now you are done. Attached in the repo is also a copy of a gdns and gdnlib file to import the lua module into godot. 
When importating the module, make sure that liblua.a for linux/mac or liblua.dll for windows is in the same folder as the compiled LuaScript module


## Using in godot
All you need to get it set up is attach the gdns script to a node (we will call this LuaNode)

lets say you have a simple lua script under res://lua_scripts/utils.lua
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
in godot to load in and execute this function all it would be is
```
onready var Lua = $"LuaNode"

func foo():
    Lua.load("res://lua_scripts/utils.lua")
    var sum = Lua.execute("sum", [1, 2, 3, 4])
```
sum will now equal 10
