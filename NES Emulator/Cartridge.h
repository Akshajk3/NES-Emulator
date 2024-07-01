#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

#include "Mapper_000.h"

class Cartridge
{
public:
    Cartridge(const std::string& FileName);
    ~Cartridge();

public:
    bool ImageValid();

    enum MIRROR
    {
        HORIZONTAL,
        VERTICAL,
        ONESCREEN_LO,
        ONESCREEN_HI,
    } mirror = HORIZONTAL;

private:
    bool imageValid = false;
    
    std::vector<uint8_t> PRGMemory;
    std::vector<uint8_t> CHRMemory;

    uint8_t MapperID = 0;
    uint8_t PRGBanks = 0;
    uint8_t CHRBanks = 0;

    std::shared_ptr<Mapper> Mapper;
    
public:
    bool cpuRead(uint16_t addr, uint8_t &data);
    bool cpuWrite(uint16_t addr, uint8_t data);
    
    bool ppuRead(uint16_t addr, uint8_t &data);
    bool ppuWrite(uint16_t addr, uint8_t data);
};
