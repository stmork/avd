//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdlib>
#include <cstdio>

#include "dummyi2cdevice.h"

std::vector<std::uint8_t> tx_data   = { 0x8a, 0x00, 0x01, 0x02 };
std::vector<std::uint8_t> rx_header = { 0x8b, 0x00,  };
std::vector<std::uint8_t> rx_data   = { 0 };

int main()
{
	DummyI2cDevice device;

	device.send(tx_data);
	device.set_dummy(0xa5);
	device.receive(rx_header, rx_data);
	return EXIT_SUCCESS;
}
