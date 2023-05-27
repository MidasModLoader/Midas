//
// Created by xgladius on 4/17/2023.
//

#ifndef MIDAS_COMMON_H
#define MIDAS_COMMON_H

#include <cstdint>
#include <string>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>

namespace KI {
    struct wstring {
        wchar_t buffer[8];
        int64_t len; // len of string (not in bytes, but in characters)
        int64_t capacity; // capacity of alloc'd buffer
        std::wstring get_text()
        {
            if (len == 0x0 || len >= 0x100000 || capacity >= 0x100000)
            {
                return std::wstring(L"");
            }
            wchar_t *text = (wchar_t *)(buffer);
            if (capacity > 0x7)
            {
                text = (wchar_t *)(*(int64_t*)(buffer));
            }
            return std::wstring(text);
        }

        void write_text(const std::wstring& text) {
            if (text.size() > 0x7 && len > 0x7) // 0x7 because we need to add null term
            {
                memcpy((void*)(*reinterpret_cast<int64_t*>(buffer)), (void*)text.data(), text.size() * sizeof(wchar_t));
                *(int8_t*)(text.size() * sizeof(wchar_t)) = 0;
                *(int8_t*)(text.size() * sizeof(wchar_t) + 1) = 0;
            }
            else if (text.size() <= 7)
            {
                memcpy((void *)buffer, (void *)text.data(), text.size());
            }
        }
    };

    struct string {
        char buffer[16];
        int64_t len; // len of string (not in bytes, but in characters)
        int64_t capacity; // capacity of alloc'd buffer
        std::string get_text()
        {
            if (len == 0x0)
            {
                return std::string("");
            }
            char *text = buffer;
            if (len > 0xF)
            {
                text = *(char **)(buffer);
            }
            return std::string(text);
        }

        void write_text(std::string text) {
            if (text.size() > 0xF && len > 0xF)
            {
                memcpy((void*)(*reinterpret_cast<int64_t*>(buffer)), (void*)text.data(), text.size() * sizeof(char));
            }
            else if (text.size() <= 0xF)
            {
                memcpy((void *)buffer, (void *)text.data(), text.size());
            }
        }
    };
}

#endif //MIDAS_COMMON_H
