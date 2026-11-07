#include <cstdio>

#include "dummyi2cdevice.h"

void DummyI2cDevice::clock(const std::uint8_t scl)
{
	state = (state & SDA) | (scl ? SCL : 0);
	print();
}

void DummyI2cDevice::set_data(const std::uint8_t sda)
{
	state = (state & SCL) | (sda ? SDA : 0);
	print();
}

std::uint8_t DummyI2cDevice::get_ack() const
{
	return 1;
}

std::uint8_t DummyI2cDevice::get_data() const
{
	const std::uint8_t result = byte & bit;

	print();
	bit >>= 1;
	return result;
}

void DummyI2cDevice::print(const char * text) const
{
	printf("%s\n", text);
}

void DummyI2cDevice::set_dummy(const std::uint8_t dummy)
{
	byte = dummy;
	bit  = 0x80;
	print();
}

void DummyI2cDevice::print() const
{
	printf("state: %d%d # dummy: %02x %02x\n",
		(state & SCL) != 0 ? 1 : 0,
		(state & SDA) != 0 ? 1 : 0, byte, bit);
}
