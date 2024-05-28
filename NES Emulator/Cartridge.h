#pragma once

#include <cstdint>

class Cartridge
{
public:
    Cartridge();
    ~Cartridge();
    
public:
    bool cpuRead(uint16_t addr, uint8_t &data);
    bool cpuWrite(uint16_t addr, uint8_t data);
    
    bool ppuRead(uint16_t addr, uint8_t &data);
    bool ppuWrite(uint16_t addr, uint8_t data);
};
