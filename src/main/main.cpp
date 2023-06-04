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

    PropertyClass::initialize_lua(L);
    Window::initialize_lua(L);

    luabridge::getGlobalNamespace(L)
        .addFunction(
            "GetRootWindow", +[]()
                             { 
                                printf("Root window is %llx\n", root_window);
                                return root_window; });

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