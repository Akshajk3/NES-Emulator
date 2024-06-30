#pragma once

#include <cstdint>
#include <memory>

#include "Cartridge.h"

#include "olcPixelGameEngine.h"

class olc2C02
{
public:
    olc2C02();
    ~olc2C02();

private:
    uint8_t tblName[2][1024];
    uint8_t tblPalette[32];
    uint8_t tblPattern[2][4096]; // Not used by NES games but can be used in future  

public:
    // Communication with Main BUS
    uint8_t cpuRead(uint16_t addr, bool readonly = false);
    void cpuWrite(uint16_t addr, uint8_t data);

    // Communication with PPU Bus
    uint8_t ppuRead(uint16_t addr, bool readonly = false);
    void ppuWrite(uint16_t addr, uint8_t data);

private:
    std::shared_ptr<Cartridge> cart;

public:
    void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
    void clock();

private:
    olc::Pixel palScreen[0x40];
    olc::Sprite Screen = olc::Sprite(256, 240);
    olc::Sprite NameTables[2] = { olc::Sprite(256, 240), olc::Sprite(256, 240) };
    olc::Sprite PatternTables[2] = { olc::Sprite(128, 128), olc::Sprite(128, 128) };

public:
    // Debugging Utility
    olc::Sprite& GetScreen();
    olc::Sprite& GetNameTable(uint8_t index);
    olc::Sprite& GetPatternTable(uint8_t index, uint8_t pallete);
    olc::Pixel& GetColorFromPalleteRam(uint8_t pallete, uint8_t pixel);
    bool frame_complete = false;

private:
    int16_t scanLine = 0;
    int16_t cycle = 0;
};