#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "common.h"

struct window_t
{
    int64_t **vftable;
    window_t *self;
    char pad_0010[24]; // 0x0010
    float N00000402;   // 0x0028
    float N00000E5C;   // 0x002C
    char pad_0030[24]; // 0x0030
    float N00000A81;   // 0x0048
    int32_t N00000A7E; // 0x004C
    std::string m_sName;
    std::vector<std::shared_ptr<window_t>> children;
    char pad_0080[8]; // 0x0080
    window_t *parent;
    char pad_0090[8];         // 0x0090
    int32_t m_Style;          // 0x0098
    int32_t m_Flags;          // 0x009C
    int32_t m_Left;           // 0x00A0
    int32_t m_Top;            // 0x00A4
    int32_t m_Right;          // 0x00A8
    int32_t m_Bottom;         // 0x00AC
    int32_t m_ParentOffset_x; // 0x00B0
    int32_t m_ParentOffset_y; // 0x00B4
    char pad_00B8[8];         // 0x00B8
    int32_t offset_x;         // 0x00C0
    int32_t offset_y;         // 0x00C4
    float scale_x;            // 0x00C8
    float scale_y;            // 0x00CC
    uint32_t N00000417;       // 0x00D0
    float m_fTargetAlpha;     // 0x00D4
    float m_fDisabledAlpha;   // 0x00D8
    float N000008E4;          // 0x00DC
    float screen_time;        // 0x00E0 // seconds this window has been displayed on screen
    float screen_time_;       // 0x00E4 // seconds this window has been displayed on screen
    int64_t m_pWindowStyle;   // 0x00E8
    void *soundDescriptor;    // 0x00F0
    int64_t m_sHelp;          // 0x00F8
    char pad_0100[96];        // 0x0100
    int64_t m_sScript;        // 0x0160
    int64_t m_sTip;           // 0x0168
    char pad_0170[16];        // 0x0170
    int32_t m_Visible;        // 0x0180
    float N00000959;          // 0x0184
    float N0000042E;          // 0x0188
    float N00000AF7;          // 0x018C
    float N0000042F;          // 0x0190
    float N00000AFA;          // 0x0194
    char pad_0198[8];         // 0x0198
    int32_t N00000431;        // 0x01A0
    char pad_01A4[4];         // 0x01A4
    void *m_BubbleList;       // 0x01A8
    char pad_01B0[16];        // 0x01B0
    void *artDescriptor;      // 0x01C0
    char pad_01C8[120];       // 0x01C8
    char **internalClassName; // 0x0240
    std::wstring labelText;   // 0x0248
    std::wstring editText;    // 0x0268

    std::string get_class_name_from_ctor()
    {
        int64_t ctor = (int64_t)vftable[0];
        if (*(uint8_t *)ctor == 0xE9) // follow the jump
        {
            ctor += (*(int32_t *)(ctor + 1)) + 5;
        }
        const auto lea = ctor + 0x3F;
        const auto rip = *(uint32_t *)(lea + 0x3);
        return std::string((char *)(lea + rip + 7));
    }

    void chat_window_test(int padding = 0)
    {
        const auto internal_name = m_sName;
        if (internal_name == "chatLog")
        {
            const auto history = editText;
            if (history.size() > 0 &&
                history.find(L"DBGM") == std::wstring::npos && history.find(L"DBGL") == std::wstring::npos && history.find(L"WARN") == std::wstring::npos && history.find(L"ERRO") == std::wstring::npos && history.find(L"STAT") == std::wstring::npos)
            {
                chat_msg_parser_t *chat_msg_parser = new chat_msg_parser_t(history);
                // chat_msg_parser->test();
                editText = chat_msg_parser->serialize();
                delete chat_msg_parser;
            }
        }
        for (auto &child : children)
        {
            child->chat_window_test(padding + 2);
        }
    }

    std::string get_name()
    {
        return m_sName;
    }

    std::vector<std::shared_ptr<window_t>> get_children()
    {
        return children;
    }

    bool is_edit()
    {
        const auto name = get_class_name_from_ctor();
        return name == "ControlEdit" || name == "ControlButton";
    }

    bool is_label()
    {
        const auto name = get_class_name_from_ctor();
        return name == "ControlText";
    }

    int get_text(lua_State *L)
    {
        if (is_label())
        {
            printf("parent: %llx\n", this);
            const auto label_text = labelText;
            if (label_text.empty())
            {
                luabridge::push(L, luabridge::LuaNil());
            }
            else
            {
                luabridge::push(L, label_text);
            }
            return 1;
        }
        else if (is_edit())
        {
            printf("parent: %llx\n", this);
            const auto edit_text = editText;
            if (edit_text.empty())
            {
                luabridge::push(L, luabridge::LuaNil());
            }
            else
            {
                luabridge::push(L, edit_text);
            }
            return 1;
        }
        luabridge::push(L, luabridge::LuaNil());
        return 1;
    }

    int get_class_name(lua_State *L)
    {
        luabridge::push(L, get_class_name_from_ctor().c_str());
        return 1;
    }

    void set_text(const std::wstring &text)
    {
        if (is_label())
        {
            labelText = text;
        }
        else if (is_edit())
        {
            editText = text;
        }
    }

    window_t *get_parent()
    {
        return parent;
    }
};