#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

class Cartridge
{
public:
    Cartridge(const std::string& FileName);
    ~Cartridge();

private:
    std::vector<uint8_t> PRGMemory;
    std::vector<uint8_t> CHRMemory;

    uint8_t MapperID = 0;
    uint8_t PRGBanks = 0;
    uint8_t CHRBanks = 0;
    
public:
    bool cpuRead(uint16_t addr, uint8_t &data);
    bool cpuWrite(uint16_t addr, uint8_t data);
    
    bool ppuRead(uint16_t addr, uint8_t &data);
    bool ppuWrite(uint16_t addr, uint8_t data);
};
