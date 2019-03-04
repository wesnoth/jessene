#include "lua.hpp"

using namespace godot;

void LuaScript::_register_methods() {
    register_method("execute", &LuaScript::execute);
    register_method("load",&LuaScript::load);
    register_method("pushVariant",&LuaScript::pushGlobalVariant);
}

void LuaScript::_init() {
    L = luaL_newstate();
    luaL_openlibs(L);
    //example of pushing c++ function
    lua_register(L,"cSum",cSum);
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
    }
    file->close();
    return true;
}

Variant LuaScript::execute(String name, Array array) {
    lua_getglobal(L,name.alloc_c_string());
    for (int i = 0; i < array.size(); i++) {
        Variant var = array[i];
        pushVariant(var);
    }

    lua_pcall(L,array.size(),LUA_MULTRET,0);
    int numReturns = lua_gettop(L);

    if (numReturns) {
        Array results;
        for(int i = 0; i < numReturns; i++) {
            results.append(popVariant());
        }
        return results;
    } else {
        return Variant();
    }
}

bool LuaScript::pushVariant(Variant var) {
    switch (var.get_type())
    {
        case Variant::Type::STRING:
            lua_pushstring(L, (var.operator godot::String().alloc_c_string()));
            break;
        case Variant::Type::INT:
            lua_pushinteger(L, (int64_t)var);
            break;
        case Variant::Type::REAL:
            lua_pushnumber(L,var.operator double());
            break;
        case Variant::Type::BOOL:
            lua_pushboolean(L, (bool)var);
            break;
        case Variant::Type::POOL_BYTE_ARRAY:
        case Variant::Type::POOL_INT_ARRAY:
        case Variant::Type::POOL_STRING_ARRAY:
        case Variant::Type::POOL_REAL_ARRAY:
        case Variant::Type::POOL_VECTOR2_ARRAY:
        case Variant::Type::POOL_VECTOR3_ARRAY:
        case Variant::Type::POOL_COLOR_ARRAY:            
        case Variant::Type::ARRAY: {
            Array array = var.operator godot::Array();
            lua_newtable(L);
            for(int i = 0; i < array.size(); i++) {
                Variant key = i+1;
                Variant value = array[i];
                pushVariant(key);
                pushVariant(value);
                lua_settable(L,-3);
            }
            break;
        }
        case Variant::Type::DICTIONARY:
            lua_newtable(L);
            for(int i = 0; i < ((Dictionary)var).size(); i++) {
                Variant key = ((Dictionary)var).keys()[i];
                Variant value = ((Dictionary)var)[key];
                pushVariant(key);
                pushVariant(value);
                lua_settable(L,-3);
            }
            break;
        case Variant::Type::VECTOR2: {
            Vector2 vector2 = var.operator godot::Vector2();
            lua_newtable(L);
            lua_pushstring(L,"X");
            lua_pushnumber(L,vector2.x);
            lua_settable(L,-3);
            lua_pushstring(L,"Y");
            lua_pushnumber(L,vector2.y);
            lua_settable(L,-3);
            break;     
        }     
        case Variant::Type::VECTOR3: {
            Vector3 vector3 = var.operator godot::Vector3();
            lua_newtable(L);
            lua_pushstring(L,"X");
            lua_pushnumber(L,vector3.x);
            lua_settable(L,-3);
            lua_pushstring(L,"Y");
            lua_pushnumber(L,vector3.y);
            lua_settable(L,-3);
            lua_pushstring(L,"Z");
            lua_pushnumber(L,vector3.z);
            lua_settable(L,-3);
            break; 
        }
        case Variant::Type::COLOR: {
            Color color = var.operator godot::Color();
            lua_newtable(L);
            lua_pushstring(L,"R");
            lua_pushnumber(L,color.r);
            lua_settable(L,-3);
            lua_pushstring(L,"G");
            lua_pushnumber(L,color.g);
            lua_settable(L,-3);
            lua_pushstring(L,"B");
            lua_pushnumber(L,color.b);
            lua_settable(L,-3);
            lua_pushstring(L,"A");
            lua_pushnumber(L,color.a);
            lua_settable(L,-3);             
            break;
        }
        default:
            Godot::print(var);
            return false;
    }
    return true;
}

bool LuaScript::pushGlobalVariant(Variant var, String name) {
    if (pushVariant(var)) {
        lua_setglobal(L,name.alloc_c_string());
        return true;
    }
    return false;
}

Variant LuaScript::popVariant() {
    Variant result = getVariant();
    lua_pop(L,1);
    return result;
}

Variant LuaScript::getVariant(int index) {
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
                Variant key = getVariant(-2);
                Variant value = getVariant(-1);
                dict[key] = value;
            }
            if (dict.size() == 2) {
                if (dict.keys().count("X") && dict.keys().count("Y")) {
                    result = Vector2(dict["X"],dict["Y"]);
                }
            } else if (dict.size() == 3) {
                if (dict.keys().count("X") && dict.keys().count("Y") && dict.keys().count("Z")) {
                    result = Vector3(dict["X"],dict["Y"],dict["Z"]);
                }
            } else if (dict.size() == 4) {
                if (dict.keys().count("R") && dict.keys().count("G") && dict.keys().count("B") && dict.keys().count("B")) {
                    result = Color(dict["R"],dict["G"],dict["B"],dict["A"]);
                }
            } else {
                result = dict;
            }
            break;
        }
        default:
            result = Variant();
    }
    return result;
}

//lua functions
int LuaScript::cSum(lua_State* L) {
    int num1 = lua_tonumber(L,1);
    int num2 = lua_tonumber(L,2);
    
    lua_pushnumber(L, num1+ num2);
    return 1;
}