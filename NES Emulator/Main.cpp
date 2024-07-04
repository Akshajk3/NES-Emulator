﻿#include <iostream>
#include <sstream>

#include "Bus.h"
#include "olc6502.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_SOUND
#include "olcPGEX_Sound.h"

class Demo_olc6502 : public olc::PixelGameEngine
{
public:
	Demo_olc6502() { sAppName = "olc6502 Demonstration"; }

	std::shared_ptr<Cartridge> cart;
	Bus nes;
	std::map<uint16_t, std::string> mapAsm;

	bool EmulationRun = false;
	float ResidualTime = 0.0f;

	uint8_t SelectedPallete = 0x00;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++)
		{
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(nes.cpuRead(nAddr, true), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", nes.cpu.status & olc6502::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", nes.cpu.status & olc6502::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", nes.cpu.status & olc6502::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", nes.cpu.status & olc6502::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", nes.cpu.status & olc6502::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", nes.cpu.status & olc6502::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", nes.cpu.status & olc6502::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", nes.cpu.status & olc6502::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(nes.cpu.pc, 4));
		DrawString(x, y + 20, "A: $" + hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(nes.cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(nes.cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		cart = std::make_shared<Cartridge>("smb.nes");

		nes.insertCartridge(cart);

		mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

		nes.reset();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		nes.controller[0] = 0x00;
		nes.controller[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
		nes.controller[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

		if (EmulationRun)
		{
			if (ResidualTime > 0.0f)
			{
				ResidualTime -= fElapsedTime;
			}
			else
			{
				ResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do {
					nes.clock();
				} while (!nes.ppu.frame_complete);
				nes.ppu.frame_complete = false;
			}
		}
		else
		{
			// Step by step emulation
			if (GetKey(olc::Key::C).bPressed)
			{
				// clock enough for one full cpu instruction
				do {
					nes.clock();
				} while (!nes.cpu.complete());
				// CPU clock runs slower than system clock
				// it may be complete for additional system clock cycles
				// Drain those out
				do {
					nes.clock();
				} while (nes.cpu.complete());
			}

			if (GetKey(olc::Key::F).bPressed)
			{
				// Clock enough to draw current frame
				do {
					nes.clock();
				} while (!nes.ppu.frame_complete);
				// Use residual clock cycles to complete current instruction
				do {
					nes.clock();
				} while (!nes.cpu.complete());
				// Reset frame completion
				nes.ppu.frame_complete = false;
			}
		}

		if (GetKey(olc::Key::R).bPressed)
		{
			nes.reset();
		}

		if (GetKey(olc::Key::SPACE).bPressed)
		{
			EmulationRun = !EmulationRun;
		}

		if (GetKey(olc::Key::P).bPressed)
		{
			(++SelectedPallete) &= 0x07;
		}

		DrawCpu(516, 2);
		//DrawCode(516, 72, 26);

		for (int i = 0; i < 26; i++)
		{
			std::string s = hex(i, 2) + ": (" + std::to_string(nes.ppu.pOAM[i * 4 + 3])
				+ ", " + std::to_string(nes.ppu.pOAM[i * 4 + 0]) + ") "
				+ "ID: " + hex(nes.ppu.pOAM[i * 4 + 1], 2) +
				+" AT: " + hex(nes.ppu.pOAM[i * 4 + 2], 2);
			DrawString(516, 72 + i * 10, s);
		}

		const int SwatchSize = 6;
		for (int p = 0; p < 8; p++) // Each Pallete
		{
			for (int s = 0; s < 4; s++) // Each Index
			{
				FillRect(516 + p * (SwatchSize * 5) + s * SwatchSize, 340,
					SwatchSize, SwatchSize, nes.ppu.GetColorFromPalleteRam(p, s));
			}
		}

		DrawRect(516 + SelectedPallete * (SwatchSize * 5) - 1, 339, (SwatchSize * 4), SwatchSize, olc::WHITE);

		DrawSprite(516, 348, &nes.ppu.GetPatternTable(0, SelectedPallete));
		DrawSprite(648, 348, &nes.ppu.GetPatternTable(1, SelectedPallete));

		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);

		//olc::Sprite& s = nes.ppu.GetPatternTable(1, SelectedPallete);
		//for (uint8_t y = 0; y < 30; y++)
		//{
		//	for (uint8_t x = 0; x < 32; x++)
		//	{
		//		DrawString(x * 16, y * 16, hex((uint32_t)nes.ppu.tblName[0][y * 32 + x], 2));
		//		uint8_t id = (uint32_t)nes.ppu.tblName[0][y * 32 + x];
		//		DrawPartialSprite(x * 16, y * 16, &s,
		//			(id & 0x0F) << 3, ((id >> 4) & 0x0F) << 3, 8, 8, 2);
		//	}
		//}

		return true;
	}
}; 

int main()
{
	Demo_olc6502 demo;
	demo.Construct(780, 480, 1, 1);
	demo.Start();
	return 0;
}
