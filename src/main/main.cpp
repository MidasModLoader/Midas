#include "main.h"

inline int traceback(lua_State *L)
{
    // look up Lua's 'debug.traceback' function
    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return 1;
    }

    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 2);
        return 1;
    }

    lua_pushvalue(L, 1);
    lua_pushinteger(L, 2);
    lua_call(L, 2, 1);

    lua_getglobal(L, "print");
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1);
        return 1;
    }

    lua_pushvalue(L, 1);
    lua_call(L, 1, 0);

    return 1;
}

inline int luaL_loadstring(lua_State *L, const char *s)
{
    std::size_t bytecodeSize = 0;

    auto bytecode = std::shared_ptr<char>(
        luau_compile(s, std::strlen(s), nullptr, &bytecodeSize),
        [](char *x)
        { std::free(x); });

    return luau_load(L, "...", bytecode.get(), bytecodeSize, 0);
}

bool runLua(const std::string &script, lua_State *state)
{
    lua_settop(state, 0);

    luabridge::lua_pushcfunction_x(state, &traceback);

    if (luaL_loadstring(state, script.c_str()) != LUABRIDGE_LUA_OK)
    {
        auto errorString = lua_tostring(state, -1);
        std::cerr << "===== Lua Compile Error =====" << std::endl;
        std::cerr << errorString << std::endl;
        return false;
    }

    if (lua_pcall(state, 0, 0, -2) != LUABRIDGE_LUA_OK)
    {
        auto errorString = lua_tostring(state, -1);
        std::cerr << "===== Lua Call Error =====" << std::endl;
        std::cerr << (errorString ? errorString : "Unknown lua runtime error") << std::endl;
        return false;
    }

    return true;
}

int init_hooks()
{
    AllocConsole();
    SetConsoleTitle("Midas");
    static_cast<void>(freopen("CONOUT$", "w", stdout));
    static_cast<void>(freopen("CONIN$", "r", stdin));
    MH_Initialize();
    hook_free();
    hook_adap();
    hook_k32();
    // hook_read();
    hook_root_window_init();
    printf("Hooks initialized\n");
    // hook_static_function_init();
    return 0;
}

std::wstring s2ws(const std::string &str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring &wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

namespace luabridge
{

    template <>
    struct Stack<std::wstring>
    {
        static Result push(lua_State *L, const std::wstring &str)
        {
            if (str.empty())
            {
                lua_pushliteral(L, "");
            }
            else
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
                std::string buf = conv.to_bytes(str);
                lua_pushlstring(L, buf.data(), buf.length());
            }
            return {};
        }

        static std::wstring get(lua_State *L, int index)
        {
            size_t len;
            const char *p = luaL_checklstring(L, index, &len);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            return conv.from_bytes(p, p + len);
        }

        static std::wstring opt(lua_State *L, int index, const std::wstring &def)
        {
            return lua_isnoneornil(L, index) ? def : get(L, index);
        }
    };
}

// login: modtest01 modtest01

int main()
{
    std::unique_ptr<lua_State, void (*)(lua_State *)> globalState(luaL_newstate(), lua_close);
    auto *L = globalState.get();
    luaL_openlibs(L);
    luaL_sandboxthread(L);

    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_setfield(L, -1, "_G");

    luabridge::getGlobalNamespace(L)
        .beginClass<window_t>("Window")
        .addFunction("GetName", &window_t::get_name)
        .addFunction("GetChildren", &window_t::get_children)
        .addFunction("IsEdit", &window_t::is_edit)
        .addFunction("IsLabel", &window_t::is_label)
        //.addFunction("SetText", &window_t::set_text)
        .addFunction("GetText", &window_t::get_text)
        .addFunction("GetClassName", &window_t::get_class_name)
        .addFunction("GetParent", &window_t::get_parent)
        .endClass()
        .addFunction(
            "GetRootWindow", +[]()
                             { return root_window; });

    std::string script;
    while (std::getline(std::cin, script))
    {
        runLua(script.c_str(), L);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, void *Reserved)
{
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        me = Module;
        init_hooks();
        main_thread.detach();
        break;
    case DLL_PROCESS_DETACH:
        if (Reserved != nullptr)
        {
            break;
        }
        main_thread.request_stop();
        break;
    default:
        break;
    }
    return TRUE;
}