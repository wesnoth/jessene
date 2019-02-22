#ifndef LUA_HPP
#define LUA_HPP

#include <Godot.hpp>
#include <Node.hpp>
#include <File.hpp>
#include <map>

extern "C" {
    #include "luasrc/lua.h"
    #include "luasrc/lauxlib.h"
    #include "luasrc/lualib.h"
}

namespace godot {
    class LuaScript: public Node {
        GODOT_CLASS(LuaScript, Node)
    public:
        static void _register_methods();

        void _init();
        LuaScript();
        ~LuaScript();

        bool load(String fileName);
        Variant execute(String name, Array array);
        String getName();
        Variant test();
        void pushVariant(lua_State* L, Variant var);
        Variant popVariant(lua_State* L);
        Variant getVariant(lua_State* L, int index = -1);

        //lua methods
        static int testing(lua_State* L);
    private:
        lua_State* L;
    };
 
}

#endif