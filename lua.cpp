#include "lua.hpp"

using namespace godot;

void LuaScript::_register_methods() {
    register_method("execute", &LuaScript::execute);
    register_method("load",&LuaScript::load);
}

void LuaScript::_init() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

LuaScript::LuaScript() {
}

LuaScript::~LuaScript() {
    lua_close(L);
}

bool LuaScript::load(String fileName) {
    File *file = File::_new();
    file->open(fileName,File::ModeFlags::READ);
    String text = file->get_as_text();
    PoolStringArray lines = text.split("\n");
    std::map<String,String> allFunctions;
    String inFunction = "";
    String functionBegin = "function";
    String functionEnd = "end";
    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i];
        if (inFunction == "") {
            if (line.begins_with(functionBegin)) {
                inFunction = true;
                int endPos = line.find("(");
                String name = line.substr(functionBegin.length(),endPos-functionBegin.length()).strip_edges();               
                allFunctions.insert(std::pair<String,String>(name,line+"\n"));
                inFunction = name;
            }
        } else {
            allFunctions[inFunction]+=(line+"\n");
            if (line == functionEnd) {
                inFunction = "";
            }
        }
    }

    for(std::pair<String,String> function : allFunctions) {
        if (luaL_loadstring(L, function.second.alloc_c_string()) || (lua_pcall(L, 0, 0 , 0))) {
            Godot::print("Error: script not loaded (" + function.first + ")");
            L = 0;
            return false;
        }
        /*example of pushing godot variables    
        lua_pushinteger(L,get_parent()->get_child(0)->get("data"));
        lua_setglobal(L, "data");
        */
        /*example of pushing c function
        lua_pushcfunction(L, testing);
        lua_setglobal(L, "Csum");
        */
    }
    file->close();
    return true;
}

Variant LuaScript::execute(String name, Array array) {
    lua_getglobal(L,name.alloc_c_string());
    for (int i = 0; i < array.size(); i++) {
        Variant var = array[i];
        pushVariant(L,var);
    }

    lua_pcall(L,array.size(),LUA_MULTRET,0);
    int numReturns = lua_gettop(L);

    if (numReturns) {
        Array results;
        for(int i = 0; i < numReturns; i++) {
            results.append(popVariant(L));
        }
        return results;
    } else {
        return Variant();
    }
}

void LuaScript::pushVariant(lua_State* L, Variant var) {
    switch (var.get_type())
    {
        case Variant::Type::STRING:
            lua_pushstring(L, (var.operator godot::String().alloc_c_string()));
            break;
        case Variant::Type::INT:
            lua_pushinteger(L, (int64_t)var);
            break;
        case Variant::Type::BOOL:
            lua_pushboolean(L, (bool)var);
            break;
        case Variant::Type::ARRAY: {
            Array array = var.operator godot::Array();
            lua_newtable(L);
            for(int i = 0; i < array.size(); i++) {
                Variant key = i+1;
                Variant value = array[i];
                pushVariant(L,key);
                pushVariant(L,value);
                lua_settable(L,-3);
            }
            break;
        }
        case Variant::Type::DICTIONARY:
            lua_newtable(L);
            for(int i = 0; i < ((Dictionary)var).size(); i++) {
                Variant key = ((Dictionary)var).keys()[i];
                Variant value = ((Dictionary)var)[key];
                pushVariant(L,key);
                pushVariant(L,value);
                lua_settable(L,-3);
            }
            break;
        default:
            break;
    }
}

Variant LuaScript::popVariant(lua_State* L) {
    Variant result = getVariant(L);
    lua_pop(L,1);
    return result;
}

Variant LuaScript::getVariant(lua_State* L, int index) {
    Variant result;
    int type = lua_type(L,index);
    switch (type) {
        case LUA_TSTRING:
            result = lua_tostring(L,index);
            break;
        case LUA_TNUMBER:
            result = lua_tonumber(L,index);
            break;
        case LUA_TBOOLEAN:
            result = (bool)lua_toboolean(L,index);
            break;
        case LUA_TTABLE:
        {
            Dictionary dict;
            for (lua_pushnil(L); lua_next(L, index-1); lua_pop(L, 1)) {
                Variant key = getVariant(L, -2);
                Variant value = getVariant(L, -1);
                dict[key] = value;
            }
            result = dict;
            break;
        }
        default:
            Godot::print(Variant(type));
            result = 0;
    }
    return result;
}

//lua functions
int LuaScript::testing(lua_State* L) {
    int num1 = lua_tonumber(L,1);
    int num2 = lua_tonumber(L,2);
    lua_pushnumber(L, num1+ num2);
    return 1;
}