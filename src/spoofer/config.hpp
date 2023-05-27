#pragma once
#include <Windows.h>
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

class HWIDConfig
{
    std::vector<uint8_t> mac;
    std::vector<uint8_t> num_smbios;
    std::vector<uint8_t> ascii_smbios;
    std::filesystem::path path;
    std::fstream file;

public:
    HWIDConfig()
    {
        path = std::filesystem::temp_directory_path().parent_path().parent_path();

        path /= "xgladius\\";
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }
        path /= "hwid_config";

        CreateFileA(std::string(path.string()).c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

        read();
    }

    void new_mac()
    {
        mac.clear();
        std::default_random_engine generator{static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
        std::uniform_int_distribution<int> distribution(0x0, 0xff);
        for (auto i = 0; i < 64; i++)
        {
            mac.push_back(distribution(generator));
        }
    }

    void new_smbios()
    {
        num_smbios.clear();
        ascii_smbios.clear();
        std::default_random_engine generator{static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};
        ;
        std::uniform_int_distribution<int> num_dist(0x30, 0x39);
        std::uniform_int_distribution<int> ascii_dist(0x41, 0x7A);
        for (auto i = 0; i < 256; i++)
        {
            num_smbios.push_back(num_dist(generator));
            ascii_smbios.push_back(ascii_dist(generator));
        }
    }

    void write()
    {
        file.open(std::string(path.string()), std::ios_base::binary | std::ios_base::out);
        for (const auto e : mac)
            file << e;
        for (const auto e : num_smbios)
            file << e;
        for (const auto e : ascii_smbios)
            file << e;
        file.close();
    }

    std::vector<uint8_t> get_mac()
    {
        return mac;
    }

    std::vector<uint8_t> get_num_smbios()
    {
        return num_smbios;
    }

    std::vector<uint8_t> get_ascii_smbios()
    {
        return ascii_smbios;
    }

    void read()
    {
        std::ifstream ifile(std::string(path.string()), std::ios_base::binary);
        if (ifile.is_open())
        {
            for (auto i = 0; i < 64; i++)
            {
                ifile.seekg(i);
                mac.push_back(ifile.get());
            }
            for (auto i = 0; i < 256; i++)
            {
                ifile.seekg(64 + i);
                num_smbios.push_back(ifile.get());
            }
            for (auto i = 0; i < 256; i++)
            {
                ifile.seekg(64 + 256 + i);
                ascii_smbios.push_back(ifile.get());
            }
        }
        else
        {
            std::cout << "Error: " << strerror(errno);
        }
    }
};