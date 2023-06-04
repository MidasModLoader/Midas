#include <Windows.h>
#include <MinHook.h>
#include <chat_msg_parser/chat_msg_parser.h>
#include <filesystem>
#include <ostream>
#include <wiz_structs/window.h>

struct wad_memory_element_t
{
    struct wad_memory_file_t
    {
        int64_t wad_pointer;
        KI::string file_name;
        const char *file_content; // content of file, see unpacked wad
        int32_t file_size;        // trailing 0x0D0A ?? (this does not include \0)

        std::vector<uint8_t> get_file_data()
        {
            const auto real_size = file_size;
            std::vector<uint8_t> ret(real_size);
            memcpy(&ret[0], file_content, real_size);
            ret.push_back(0); // null termination
            return ret;
        }
    } *memory_file;
};

std::string retrieve_string(const char *str)
{
    std::string file_name;
    if (*(uint64_t *)(str + 0x18) >= 0x10)
    {
        file_name = *(const char **)(str);
    }
    else
    {
        file_name = str;
    }
    return file_name;
}

wad_memory_element_t *(__stdcall *ResourceManagerLoadWadElement_orig)(int64_t ResourceManager, wad_memory_element_t *wad_memory_element, const char *wad_name_ptr, const char *name_ptr, uint64_t *a5);

wad_memory_element_t *
ResourceManagerLoadWadElement(int64_t ResourceManager, wad_memory_element_t *wad_memory_element, const char *wad_name_ptr, const char *name_ptr, uint64_t *a5)
{
    const auto file_name = retrieve_string(name_ptr);
    const auto wad_name = retrieve_string(wad_name_ptr) + ".wad";

    auto *ret = ResourceManagerLoadWadElement_orig(ResourceManager, wad_memory_element, wad_name_ptr, name_ptr, a5);
    if (ret->memory_file == 0)
    {
        return ret;
    }

    /*auto file_data = ret->memory_file->get_file_data();

    std::filesystem::path path{"C:\\Users\\xgladius\\Downloads\\wad_out\\" + wad_name};
    path /= file_name;
    std::filesystem::create_directories(path.parent_path());

    std::ofstream outfile(path, std::ios::out | std::ios::binary);
    outfile.write((const char *)&file_data[0], file_data.size());
    outfile.close();*/

    return ret;
}

void hook_read()
{
    const auto adr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr)) + 0x1176A30;
    MH_CreateHook(reinterpret_cast<LPVOID>(adr), &ResourceManagerLoadWadElement, reinterpret_cast<LPVOID *>(&ResourceManagerLoadWadElement_orig));
    MH_EnableHook(reinterpret_cast<LPVOID>(adr));
}

bool(__stdcall *WizardGuiManager__InitRootWindow_orig)(int64_t WizardGuiManager);

window_t *root_window = nullptr;

bool WizardGuiManager__InitRootWindow_hook(int64_t WizardGuiManager)
{
    auto current_root_window = WizardGuiManager + 0xD8;
    root_window = (window_t *)(*(uint64_t *)current_root_window);
    // root_window->chat_window_test();
    return WizardGuiManager__InitRootWindow_orig(WizardGuiManager);
}

void hook_root_window_init()
{
    const auto adr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr)) + 0x150A9F0;
    MH_CreateHook(reinterpret_cast<LPVOID>(adr), &WizardGuiManager__InitRootWindow_hook, reinterpret_cast<LPVOID *>(&WizardGuiManager__InitRootWindow_orig));
    MH_EnableHook(reinterpret_cast<LPVOID>(adr));
}

int64_t(__fastcall *StaticFunctionInit_orig)(int64_t class_ptr, const char *name, int64_t func_ref, int zero, int zero_1);

int64_t StaticFunctionInit_hook(int64_t class_ptr, const char *name, int64_t func_ref, int zero, int zero_1)
{
    printf("0x%llx, \"%s\",\n", *(int64_t *)(func_ref + 0x30) - reinterpret_cast<uint64_t>(GetModuleHandle(nullptr)) + 0x140000000, name);
    return StaticFunctionInit_orig(class_ptr, name, func_ref, zero, zero_1);
}

void hook_static_function_init()
{
    const auto adr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr)) + 0x118B4D0;
    MH_CreateHook(reinterpret_cast<LPVOID>(adr), &StaticFunctionInit_hook, reinterpret_cast<LPVOID *>(&StaticFunctionInit_orig));
    MH_EnableHook(reinterpret_cast<LPVOID>(adr));
}