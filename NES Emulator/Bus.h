#pragma once

#include <cstdint>
#include "olc6502.h"
#include "olc2C02.h"
#include <array>

class Bus
{
public:
	Bus();
	~Bus();

public: // Devices on bus
	olc6502 cpu;
    
    olc2C02 ppu;

	std::array<uint8_t, 2048> cpuRam;

public: // Bus read and write
	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr, bool ReadOnly = false);
};
