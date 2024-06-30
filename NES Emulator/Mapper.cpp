#include "Mapper.h"

Mapper::Mapper(uint8_t pPrgBanks, uint8_t pChrBanks)
{
	prgBanks = pPrgBanks;
	chrBanks = pChrBanks;

	reset();
}

Mapper::~Mapper()
{

}

void Mapper::reset()
{

}