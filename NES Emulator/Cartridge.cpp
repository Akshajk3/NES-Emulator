#include "Cartridge.h"

Cartridge::Cartridge(const std::string& fileName)
{
	// iNES Format Header
	struct Header
	{
		char name[4];
		uint8_t prg_rom_chunks;
		uint8_t chr_rom_chunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char unused[5];
	} header;

	imageValid = false;

	std::ifstream ifs;
	ifs.open(fileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		ifs.read((char*)&header, sizeof(header));

		if (header.mapper1 & 0x04)
			ifs.seekg(512, std::ios_base::cur);

		MapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

		uint8_t FileType = 1;
		
		if (FileType == 0)
		{

		}

		if (FileType == 1)
		{
			PRGBanks = header.prg_rom_chunks;
			PRGMemory.resize(PRGBanks * 16384);
			ifs.read((char*)PRGMemory.data(), PRGMemory.size());


			CHRBanks = header.chr_rom_chunks;
			CHRMemory.resize(CHRBanks * 8192);
			ifs.read((char*)CHRMemory.data(), CHRMemory.size());
		}

		if (FileType == 2)
		{

		}

		switch (MapperID)
		{
		case 0:
			Mapper = std::make_shared<Mapper_000>(PRGBanks, CHRBanks);
			break;
		}

		imageValid = true;
		ifs.close();
	}
}

Cartridge::~Cartridge()
{

}

bool Cartridge::ImageValid()
{
	return imageValid;
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr = 0;
	if (Mapper->cpuMapRead(addr, mapped_addr))
	{
		data = PRGMemory[mapped_addr];
		return true;
	}
	else
	{
		return false;
	}
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr;
	if (Mapper->cpuMapWrite(addr, mapped_addr))
	{
		PRGMemory[mapped_addr] = data;
		return true;
	}
	else
	{
		return false;
	}
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr;
	if (Mapper->cpuMapWrite(addr, mapped_addr))
	{
		data = CHRMemory[mapped_addr];
		return true;
	}
	else
	{
		return false;
	}
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr;
	if (Mapper->cpuMapWrite(addr, mapped_addr))
	{
		CHRMemory[mapped_addr] = data;
		return true;
	}
	else
	{
		return false;
	}
}