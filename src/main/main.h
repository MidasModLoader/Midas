#include <Windows.h>
#include <thread>
#include <spoofer/hooks/iphlp.hpp>
#include <spoofer/hooks/kern32.hpp>
#include <spoofer/hooks/k32_read.hpp>
#include <DbgHelp.h>
#include <locale>
#include <codecvt>

int main();

int init_hooks();

std::jthread main_thread(main);

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL, // handle to DLL module
    DWORD fdwReason,    // reason for calling function
    LPVOID lpvReserved);