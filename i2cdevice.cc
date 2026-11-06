//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdio>

#include "i2cdevice.h"

void I2CDevice::send(const std::vector<std::uint8_t> & tx_data)
{
	start();
	printf("-----\n");
	for (const auto byte : tx_data)
	{
		send(byte);
	}
	printf("-----\n");
	stop();
}

void I2CDevice::receive(const std::vector<std::uint8_t> & tx_data, std::vector<std::uint8_t> & rx_data)
{
	start();
	printf("-----\n");
	for (const auto byte : tx_data)
	{
		send(byte);
	}
	printf("=====\n");
	for (std::size_t i = 0; i < rx_data.size(); i++)
	{
		rx_data[i] = receive();
	}
	printf("-----\n");
	stop();
}

std::uint8_t I2CDevice::send(const std::uint8_t byte)
{
	for (std::uint8_t bit = 0x80; bit != 0; bit >>= 1)
	{
		clock(0);
		set_data((byte & bit) ? 1 : 0);
		clock(1);
	}
	clock(0);
	set_data(1);

	unsigned     timeout = 1000;
	std::uint8_t ack;

	do
	{
		clock(1);
		ack = get_ack();
	}
	while (!ack && (--timeout > 0));
	clock(0);
	printf(">---- %02x\n", byte);

	return ack;
}

std::uint8_t I2CDevice::receive()
{
	std::uint8_t byte = 0;

	for (std::uint8_t bit = 0x80; bit != 0; bit >>= 1)
	{
		clock(1);
		if (get_data())
		{
			byte |= bit;
		}
		clock(0);
	}
	return byte;
}

void I2CDevice::start()
{
	clock(0);
	set_data(1);
	clock(1);
	set_data(0);
}

void I2CDevice::stop()
{
	set_data(0);
	clock(0);
}
