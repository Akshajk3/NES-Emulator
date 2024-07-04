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

public:
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

    bool nmi;

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

    union
    {
        struct
        {
            uint8_t unused : 5;
            uint8_t sprite_overflow : 1;
            uint8_t sprite_zero_hit : 1;
            uint8_t vertical_blank : 1;
        };

        uint8_t reg;
    } status;

    union
    {
        struct
        {
            uint8_t grayscale : 1;
            uint8_t render_background_left : 1;
            uint8_t render_sprite_left : 1;
            uint8_t render_background : 1;
            uint8_t render_sprite : 1;
            uint8_t enhance_red : 1;
            uint8_t enhance_green : 1;
            uint8_t enhance_blue : 1;
        };

        uint8_t reg;
    } mask;

    union
    {
        struct
        {
            uint8_t nametable_x : 1;
            uint8_t nametable_y : 1;
            uint8_t increment_mode : 1;
            uint8_t pattern_sprite : 1;
            uint8_t pattern_background : 1;
            uint8_t sprite_size : 1;
            uint8_t slave_mode : 1;
            uint8_t enable_nmi : 1;
        };

        uint8_t reg;
    } control;

    uint8_t address_latch = 0x00;
    uint8_t ppu_data_buffer = 0x00;
    
    union loopy_register
    {
        struct
        {
            uint16_t coarse_x : 5;
            uint16_t coarse_y : 5;
            uint16_t nametable_x : 1;
            uint16_t nametable_y : 1;
            uint16_t fine_y : 3;
            uint16_t unused : 1;
        };
        uint16_t reg = 0x0000;
    };
    
    loopy_register vram_addr;
    loopy_register tram_addr;
    
    uint8_t fine_x = 0x00;
    
    uint8_t bg_next_tile_id = 0x00;
    uint8_t bg_next_tile_attrib = 0x00;
    uint8_t bg_next_tile_lsb = 0x00;
    uint8_t bg_next_tile_msb = 0x00;

    uint16_t bg_shifter_pattern_lo = 0x0000;
    uint16_t bg_shifter_pattern_hi = 0x0000;
    uint16_t bg_shifter_attrib_lo = 0x0000;
    uint16_t bg_shifter_attrib_hi = 0x0000;

private:
    struct ObjectAttributeEntry
    {
        uint8_t y; // Y Position of Sprite
        uint8_t id; // ID of tile from pattern memory
        uint8_t attribute; // Flags that define how sprite is rendered
        uint8_t x; // X position of Sprite
    } OAM[64];

    uint8_t oam_addr = 0x00;

    ObjectAttributeEntry spriteScanLine[8];
    uint8_t spriteCount;
    uint8_t sprite_shifter_pattern_lo[8];
    uint8_t sprite_shifter_pattern_hi[8];

    bool SpriteZeroHitPossible = false;
    bool SpriteZeroBeingRendered = false;

public:
    uint8_t* pOAM = (uint8_t*)OAM;

};
