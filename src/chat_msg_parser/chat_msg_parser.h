#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

std::vector<std::wstring> split(std::wstring s, wchar_t delimiter)
{
    std::vector<std::wstring> parts;
    std::wstringstream wss(s);
    std::wstring temp = L"";
    while (std::getline(wss, temp, delimiter))
    {
        parts.push_back(temp);
    }
    return parts;
}

int countSubstring(const std::wstring &str, const std::wstring &sub)
{
    if (sub.length() == 0)
        return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
         offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

class chat_msg_parser_t
{
    struct class_msg_t
    {
        bool valid;
        struct msg_t
        {
            std::wstring color = L"";
            std::wstring message = L"";
        } msg;

        struct player_t
        {
            std::wstring GID = L"";
            std::wstring name = L"";
            std::wstring level = L"";
            std::wstring link = L"";
        } player;

        struct img_t
        {
            std::wstring path = L"";
            std::wstring size_x = L"";
            std::wstring size_y = L"";
            std::wstring color = L"";
        } img;

        std::wstring serialize()
        {
            std::wstring ret = L"";
            ret += L"<color;" + msg.color + L"><image;" + img.path + L";" + img.size_x + L";" + img.size_y + L";" + img.color + L">";
            if (!player.GID.empty())
            {
                ret += L"<link;GID:" + player.GID + L"," + player.name + L"," + player.level + L">" + player.link + L"</link>";
            }
            ret += msg.message + L"</color>";
            return ret;
        }
    };

    std::vector<class_msg_t> messages;

public:
    class_msg_t
    process_line(std::wstring line)
    {
        class_msg_t ret = {};
        if (line.size() == 0)
        {
            ret.valid = false;
            return ret;
        }

        auto splitted = split(line, L';');
        if (splitted.size() < 6)
        {
            ret.valid = false;
            return ret;
        }
        ret.valid = true;
        ret.msg.color = splitted[1].substr(0, splitted[1].find(L">"));
        ret.img.size_x = splitted[3];
        ret.img.size_y = splitted[4];
        ret.img.color = splitted[5].substr(0, splitted[5].find(L">"));

        const auto link = line.find(L"<link;");
        if (link != std::wstring::npos)
        { // handle chat from another player
            const auto gid_loc = line.find(L"GID:") + sizeof("GID");
            const auto first_comma = line.find(L",", gid_loc);
            ret.player.GID = line.substr(gid_loc, first_comma - gid_loc);

            const auto second_comma = line.find(L",", first_comma + 1);
            ret.player.name = line.substr(first_comma + 1, second_comma - first_comma - 1);

            auto tag_end = line.find(L">", second_comma + 1);
            ret.player.level = line.substr(second_comma + 1, tag_end - second_comma - 1);

            tag_end++;
            auto link_end = line.find(L"</link>", tag_end);
            ret.player.link = line.substr(tag_end, link_end - tag_end);

            link_end += sizeof("</link>");
            const auto message = line.substr(link_end - 1, line.find(L"</color>", link_end) - link_end + 1);
            ret.msg.message = message;

            ret.img.path = L"Art/Art_Chat_Say.dds";

            return ret;
        }

        std::wstring type = L"none";
        if (countSubstring(line, L"<image;") == 2)
        {
            auto offset = line.find(L"<image;", line.find(L"<image;") + 1);
            type = line.substr(offset + sizeof("<image"), line.find(L">", offset) - sizeof("<image") - offset);
            std::wstring::size_type i = line.find(L" <image;" + type + L">");
            line.erase(i, sizeof("<image;>") + type.length());
        }

        if (type != L"none")
        {
            ret.img.path = L"QuestPage/Art_Quest_" + type + L".dds";
        }
        else
        {
            ret.img.path = splitted[2];
        }

        splitted = split(line, L';'); // update because we might have removed an image tag
        const auto message = splitted[5].substr(splitted[5].find(L">") + 1, splitted[5].find(L"<") - splitted[5].find(L">") - 1);
        if (ret.msg.color == L"AA00AA") // "You have received %d experience!"
        {
            ret.img.path = L"Art/Art_Quest_XP.dds";
        }
        else if (ret.msg.color == L"00FFFF")
        { // received pet to pet tome
            ret.img.path = L"QuestPage/Art_Quest_Pet.dds";
        }
        else if (ret.msg.color == L"D9ABF8")
        {
            ret.img.path = L"QuestPage/Art_Quest_Emote.dds";
        }
        else if (message.find(L"You have earned") != std::wstring::npos && message.find(L"gold!") != std::wstring::npos)
        {
            ret.msg.color = L"00CCFE";
            ret.img.path = L"QuestPage/Art_Quest_Gold.dds";
        }
        ret.msg.message = message;
        return ret;
    }

    chat_msg_parser_t(std::wstring chat_history)
    {
        const auto split_lines = split(chat_history, L'\n');
        for (auto line : split_lines)
        {
            printf("%ws\n", line.c_str());
            auto ret = process_line(line);
            if (ret.valid)
            {
                messages.push_back(ret);
            }
        }
    }

    std::wstring serialize()
    {
        std::wstring ret = L"";
        for (auto i = 0; i < messages.size(); i++)
        {
            ret += messages[i].serialize() + (i == messages.size() - 1 ? L"" : L"\n");
        }
        return ret;
    }

    void test()
    {
        for (auto i = 0; i < messages.size(); i++)
        {
            messages[i].msg.color = L"00CCFE";
        }
    }
};