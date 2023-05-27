#pragma once
#include <Windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <MinHook.h>
#include "../config.hpp"

ULONG(__stdcall *GetAdaptersInfo_orig)
(PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer);

ULONG __stdcall GetAdaptersInfo_hook(PIP_ADAPTER_INFO adapter, PULONG size)
{
    printf("mac issue\n");
    const auto ret = GetAdaptersInfo_orig(adapter, size);
    if (ret != NO_ERROR)
    {
        return ret;
    }

    HWIDConfig conf;
    const auto mac = conf.get_mac();

    do
    {
        for (auto i = 0; i < adapter->AddressLength; i++)
        {
            adapter->Address[i] = mac.at(i);
        }
        adapter = adapter->Next;
    } while (adapter);

    return ret;
}

void hook_adap()
{
    auto Iphlapi = GetModuleHandle("Iphlpapi.dll");
    while (!Iphlapi)
    {
        Iphlapi = LoadLibrary("Iphlpapi.dll");
        Sleep(10);
    }
    const LPVOID gai_address = GetProcAddress(Iphlapi, "GetAdaptersInfo");

    MH_CreateHook(gai_address, &GetAdaptersInfo_hook, reinterpret_cast<LPVOID *>(&GetAdaptersInfo_orig));
    MH_EnableHook(gai_address);
}