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
        bool pushVariant(Variant var);
        bool pushGlobalVariant(Variant var, String name);
        Variant popVariant();
        Variant getVariant(int index = -1);

        //lua methods
        static int cSum(lua_State* L);
    private:
        lua_State* L;

    };
 
}

#endif