//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdio>

#include "i2cdevice.h"

void I2CDevice::send(const std::vector<std::uint8_t> & tx_data)
{
	start();
	print("----- tx");
	for (const auto byte : tx_data)
	{
		if (!send(byte))
		{
			stop();
			return;
		}
	}
	stop();
}

void I2CDevice::receive(
	const std::vector<std::uint8_t> & tx_data,
	std::vector<std::uint8_t>    &    rx_data)
{
	start();
	print("----- tx");
	for (const auto byte : tx_data)
	{
		if (!send(byte))
		{
			stop();
			return;
		}
	}
	print("===== rx");
	for (std::size_t i = 0; i < rx_data.size(); i++)
	{
		rx_data[i] = receive((i + 1) < rx_data.size());
	}
	stop();
}

void I2CDevice::print(const char * text) const
{
	(void)text;
}

std::uint8_t I2CDevice::send(const std::uint8_t byte)
{
	clock(0);
	for (std::uint8_t bit = 0x80; bit != 0; bit >>= 1)
	{
		set_data(byte & bit);
		clock(1);
		clock(0);
	}

	unsigned     to = 1;
	std::uint8_t ack;

	set_data(1);
	clock(1);
	do
	{
		ack = get_ack();
	}
	while (!ack && (to++ < timeout));
	clock(0);
	print(">----");

	return ack;
}

std::uint8_t I2CDevice::receive(const bool ack)
{
	std::uint8_t byte = 0;

	clock(0);
	set_data(1);
	for (std::uint8_t bit = 0x80; bit != 0; bit >>= 1)
	{
		clock(1);
		if (get_data())
		{
			byte |= bit;
		}
		clock(0);
	}

	print(ack ? "----- ACK" : "----- NAK");
	set_data(ack ? 0 : 1);
	clock(1);
	clock(0);

	return byte;
}

void I2CDevice::start()
{
	print("----- START");
	clock(0);
	set_data(1);
	clock(1);
	set_data(0);
}

void I2CDevice::stop()
{
	print("----- STOP");
	clock(0);
	set_data(0);
	clock(1);
	set_data(1);
	print("----- <<");
}
